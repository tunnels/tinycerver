#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	int status;
	char ipstring[INET_ADDRSTRLEN];
	
	// initialize hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	status = getaddrinfo("localhost", "4444", &hints, &res);
	
	if (status != 0) {
		printf("error in getaddrinfo(): %s\n", gai_strerror(status));
		return 0;
	}
	
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	inet_ntop(AF_INET, (void *)&(addr->sin_addr), ipstring, INET_ADDRSTRLEN);
	
	printf("address: %s\n", ipstring);
	
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		printf("error in socket creation()\n");
	};
	
	status = (connect(sockfd, res->ai_addr, res->ai_addrlen));
	
	if (status != 0) {
		perror("error");
	}
	
	return status;
}
