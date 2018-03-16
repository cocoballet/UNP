/*
 * 6.1_cgi.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月14日 星期三 14时28分40秒
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address, port_name\n", basename(argv[0]));
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(sock >= 0);

	int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);

	ret = listen(sock, 5);
	assert(ret != -1);

	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int connfd = accept(sock, (struct sockaddr*)&client, &client_len);
	if(connfd < 0) {
		printf("connection failure, errno: %d\n", errno);
	} else {
		close(STDOUT_FILENO);
		dup(connfd);
		printf("abc\n");
		close(connfd);
	}

	close(sock);
	return 0;
}
