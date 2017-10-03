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

int createConnection(int sockfd, struct sockaddr_storage incoming);
int attemptFileRetrieval(int connectfd);
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

	printf("listening on: %s\n", ipstring);
	
	createConnection(sockfd, incoming);

	return 0;
}

int createConnection(int sockfd, struct sockaddr_storage incoming)
{
	int status;
	int stop = 0;
	socklen_t size_of_incoming = sizeof(incoming);
	
	// char *exit_message = "exit";
	// size_t exit_message_len = strlen(exit_message);

	while (stop == 0) {
		// getRequest() to be defined
		char message_buffer[MESSAGE_MAX_LEN] = "";

		int connectfd = accept(sockfd, (struct sockaddr *)&incoming, &size_of_incoming);
		checkErrors(4, connectfd);

		status = recv(connectfd, (void *)message_buffer, MESSAGE_MAX_LEN, 0);
		checkErrors(5, status);

		attemptFileRetrieval(connectfd);

		printf("connection recorded\n");
		close(connectfd);
    
		// printf("message received: %s", message_buffer);
		// if (strncmp(message_buffer, exit_message, exit_message_len) == 0) {
		// 	printf("action: terminating connection\n");
		// 	close(sockfd);
		// 	break;
		// }
	}

	return 0;
}

int attemptFileRetrieval(int connectfd)
{
	char* http_response = getStatusGivenCode(500);
	const char filename[] = "test.txt";
	FILE *file_pointer = fopen(filename, "rb");
	int status = 0;
	http_response = getStatusGivenCode(500);

	if (file_pointer == NULL) {
		http_response = getStatusGivenCode(404);
	}
	else {
		http_response = getStatusGivenCode(200);	
		printf("retrieving file: %s\n", filename);
		
		uint8_t *file_contents = getFileContentsFromFilePointer(file_pointer);
		int response_length = strlen(http_response);
		write(connectfd, http_response, response_length); // 200 response header

		// actual file content
		status = write(connectfd, file_contents, MESSAGE_MAX_LEN);	
		
		return 0;
	}

	int response_length = strlen(http_response);
	status = write(connectfd, http_response, response_length);

	checkErrors(6, status);
	return 0;
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
