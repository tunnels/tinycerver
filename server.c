#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CONNECTION_QUEUE_MAX 5
#define MESSAGE_MAX_LEN 300

int getConnection(int sockfd, struct sockaddr_storage incoming);
void checkErrors(int errType, int status);

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
	
	getConnection(sockfd, incoming);

	return 0;
}

int getConnection(int sockfd, struct sockaddr_storage incoming)
{
	int status;
	int stop = 0;
	socklen_t sizeOfIncoming = sizeof(incoming);
	
	char *exitText = "exit";
	size_t exitTextLen = strlen(exitText);

	while (stop == 0) {
		// getRequest() to be defined
		char messageBuffer[MESSAGE_MAX_LEN] = "";

		int connectfd = accept(sockfd, (struct sockaddr *)&incoming, &sizeOfIncoming);
		checkErrors(4, connectfd);

		status = recv(connectfd, (void *)messageBuffer, MESSAGE_MAX_LEN, 0);
		checkErrors(5, status);

		const char* status_not_found = 
			"HTTP/1.1 404 Not Found\r\n"
			"Content-type: text/html\r\n"
			"\r\n"
			"<html>\r\n"
			"	<body>\r\n"
			"		<h1>Not Found</h1>\r\n"
			"		<p>The requested URL was not found on this server.</p>\r\n"
			"	</body>\r\n"
			"</html>\r\n";

		int responseLength = strlen(status_not_found);
		status = send(connectfd, status_not_found, responseLength, 0);
		checkErrors(6, status);

		printf("connection recorded\n");
		close(connectfd);
    
		// printf("message received: %s", messageBuffer);
		// if (strncmp(messageBuffer, exitText, exitTextLen) == 0) {
		// 	printf("action: terminating connection\n");
		// 	close(sockfd);
		// 	break;
		// }
	}

	return 0;
}

void checkErrors(int errType, int status)
{
	if (errType == 0) {
		if (status != 0) {
			printf("error in getaddrinfo(): %s\n", gai_strerror(status));
			exit(0);
		}
	}
	else if (errType >= 1 && errType <= 6) {
		if (status == -1) {
			perror("error"); 
			exit(0);
		}
	}

	return;
}
