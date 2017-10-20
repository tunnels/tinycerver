#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "http_codes.h"

#define CONNECTION_QUEUE_MAX 5
#define MESSAGE_MAX_LEN 300
#define URI_START_INDEX 4

int createConnection(int sockfd, struct sockaddr_storage incoming);
char *getRequestURI(char *messageBuffer);
int attemptFileRetrieval(int connectfd, char *uri);
void sendHttpHeader(int connectfd, int code);
uint8_t *getFileContentsFromFilePointer(FILE *fp);
void checkErrors(int error_type, int status);

int main(int argc, char *argv[])
{
	int status;
	struct addrinfo hints = {0};
	struct addrinfo *response = NULL;
	char ipstring[INET_ADDRSTRLEN];
	struct sockaddr_storage incoming = {0};
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	status = getaddrinfo("localhost", "4444", &hints, &response);
	checkErrors(0, status);

	struct sockaddr_in *addr = (struct sockaddr_in *)response->ai_addr;
	inet_ntop(AF_INET, (void *)&(addr->sin_addr), ipstring, INET_ADDRSTRLEN);

	int sockfd = socket(response->ai_family, response->ai_socktype, response->ai_protocol);
	checkErrors(1, sockfd);

	status = bind(sockfd, response->ai_addr, response->ai_addrlen);
	checkErrors(2, status);

	status = listen(sockfd, CONNECTION_QUEUE_MAX);
	checkErrors(3, status);
	
	createConnection(sockfd, incoming);

	return 0;
}

int createConnection(int sockfd, struct sockaddr_storage incoming)
{
	int status;
	int stop = 0;
	socklen_t size_of_incoming = sizeof(incoming);

	while (stop == 0) {
		char message_buffer[MESSAGE_MAX_LEN] = "";
		char *uri = "";

		int connectfd = accept(sockfd, (struct sockaddr *)&incoming, &size_of_incoming);
		checkErrors(4, connectfd);

		status = recv(connectfd, (void *)message_buffer, MESSAGE_MAX_LEN, 0);
		checkErrors(5, status);

		uri = getRequestURI(message_buffer);
		
		// malformed request, no need to attempt file retrieval
		if (strcmp(uri, "") == 0) {
			sendHttpHeader(connectfd, 400);
		}
		else {
			uri = (uri + 1); // skip the initial '/'
			attemptFileRetrieval(connectfd, uri);
		}
		
		close(connectfd);
	}

	return 0;
}

char *getRequestURI(char *message_buffer)
{
	int start = URI_START_INDEX;
	int end = -1;
	int length_minus_GET_prefix = MESSAGE_MAX_LEN - URI_START_INDEX;

	for (int i = URI_START_INDEX; i < length_minus_GET_prefix; i++) {
		if ( *(message_buffer + i) == ' ') {
			end = i;
			break;
		}
	}

	if (end == -1) {
		// malformed request since there was no space in the header
		return ""; 
	}

	int difference = end - start;

	char *uri = malloc(difference + 1); // extra space for null terminator
	strncpy(uri, message_buffer + URI_START_INDEX, difference);
	*(uri + difference) = '\0'; // end uri with null terminator

	return uri;
}

int attemptFileRetrieval(int connectfd, char *uri)
{
	char* http_response = getStatusGivenCode(500);
	printf("uri: %s\n", uri);

	if (strcmp(uri, "") == 0) {
		uri = "index.html";
		printf("new uri: %s\n", uri);
	}

	FILE *file_pointer = fopen(uri, "rb");
	int status = 0;
	http_response = getStatusGivenCode(500);

	if (file_pointer == NULL) {
		// http_response = getStatusGivenCode(404);
		sendHttpHeader(connectfd, 404);
		return -1;
	}
	else {
		http_response = getStatusGivenCode(200);	
		printf("retrieving file: %s\n\n", uri);
		
		uint8_t *file_contents = getFileContentsFromFilePointer(file_pointer);
		// int response_length = strlen(http_response);

		sendHttpHeader(connectfd, 200);
		status = write(connectfd, file_contents, MESSAGE_MAX_LEN);	// actual file content
		
		return 0;
	}
}

void sendHttpHeader(int connectfd, int code) {
	char *http_response = getStatusGivenCode(code);	
	int response_length = strlen(http_response);

	int status = write(connectfd, http_response, response_length);
	checkErrors(6, status);
}

uint8_t *getFileContentsFromFilePointer(FILE *file_pointer)
{
	uint8_t *file_contents = malloc(300);
	fread(file_contents, 1, MESSAGE_MAX_LEN, file_pointer);

	return file_contents;
}

void checkErrors(int error_type, int status)
{
	if (error_type == 0) {
		if (status != 0) {
			printf("error in getaddrinfo(): %s\n", gai_strerror(status));
			exit(0);
		}
	}
	else if (error_type >= 1 && error_type <= 6) {
		if (status == -1) {
			perror("error"); 
			exit(0);
		}
	}

	return;
}
