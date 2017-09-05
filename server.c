#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CONNECTION_QUEUE_MAX 2
#define MESSAGE_MAX_LEN 300

int main(int argc, char *argv[])
{
	struct addrinfo hints = {0};
	struct addrinfo *response = NULL;
	int status;
	char ipstring[INET_ADDRSTRLEN];
	struct sockaddr_storage incoming;
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	// only working locally for now
	status = getaddrinfo("localhost", "4444", &hints, &response);
	
	if (status != 0) {
		printf("error in getaddrinfo(): %s\n", gai_strerror(status));
		return 0;
	}
	
	struct sockaddr_in *addr = (struct sockaddr_in *)response->ai_addr;
	inet_ntop(AF_INET, (void *)&(addr->sin_addr), ipstring, INET_ADDRSTRLEN);

	int sockfd = socket(response->ai_family, response->ai_socktype, response->ai_protocol);
	if (sockfd == -1) {
		printf("error in socket creation()\n");
	};
	
	int bindStatus;
	int listenStatus;

	// error checking
	bindStatus = bind(sockfd, response->ai_addr, response->ai_addrlen);
	if (bindStatus != 0) {
		perror("error"); 
		return -1;
	}

	listenStatus = listen(sockfd, CONNECTION_QUEUE_MAX);
	if (listenStatus != 0) {
		perror("error"); 
		return -1;
	}

	int connectfd;
	socklen_t size = sizeof(incoming);
	printf("listening on: %s\n", ipstring);

	char *exitText = "exit";
	size_t exitTextLen = strlen(exitText);

	while (0 != 1) {
		connectfd = accept(sockfd, (struct sockaddr *)&incoming, &size);
		char msg_buffer[MESSAGE_MAX_LEN] = "";
		status = recv(connectfd, (void *)msg_buffer, MESSAGE_MAX_LEN, 0);

		printf("message received: %s", msg_buffer);
		if (strncmp(msg_buffer, exitText, exitTextLen) == 0) {
			printf("action: terminating connection\n");
			close(sockfd);
			break;
		}
	}

	return status;
}
