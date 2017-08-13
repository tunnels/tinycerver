#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CONNECTION_QUEUE_MAX 7

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
	
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, CONNECTION_QUEUE_MAX);

	int connectfd;
	int size = sizeof(incoming);
	printf("listening on: %s\n", ipstring);
	connectfd = accept(sockfd, (struct sockaddr *)&incoming, &size);

	if (status != 0) {
		perror("error");
	}
	
	return status;
}
