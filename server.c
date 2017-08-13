#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CONNECTION_QUEUE_MAX 2
#define MESSAGE_MAX_LEN 300

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	int status;
	char ipstring[INET_ADDRSTRLEN];
	struct sockaddr_storage incoming;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	// only working locally for now
	status = getaddrinfo("localhost", "4444", &hints, &res);
	
	if (status != 0) {
		printf("error in getaddrinfo(): %s\n", gai_strerror(status));
		return 0;
	}
	
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, (void *)&(addr->sin_addr), ipstring, INET_ADDRSTRLEN);
	
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		printf("error in socket creation()\n");
	};
	
	int bindstatus;
	int listenstatus;

	// error checking
	bindstatus = bind(sockfd, res->ai_addr, res->ai_addrlen);
	if (bindstatus != 0) {
		perror("error"); 
		return -1;
	}

	listenstatus = listen(sockfd, CONNECTION_QUEUE_MAX);
	if (listenstatus != 0) {
		perror("error"); 
		return -1;
	}

	int connectfd;
	socklen_t size = sizeof(incoming);
	printf("listening on: %s\n", ipstring);

	while (0 != 1) {
		connectfd = accept(sockfd, (struct sockaddr *)&incoming, &size);

		char msg_buffer[MESSAGE_MAX_LEN];
		status = recv(connectfd, (void *)msg_buffer, MESSAGE_MAX_LEN, 0);

		printf("message received: %s", msg_buffer);
	}

	return status;
}
