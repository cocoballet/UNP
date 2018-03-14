/*
 * 5.11_recvBuff.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月14日 星期三 09时45分33秒
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address, port_number, recv_buffer_size\n", basename(argv[0]));
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

	int recvbuf = atoi(argv[3]);
	int len = sizeof(recvbuf);

	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf));
	getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, &len);
	printf("tcp recv_buf_size is %d after setting\n", recvbuf);

	int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);

	ret = listen(sock, 5);
	assert(ret != -1);

	struct sockaddr_in client;
	socklen_t client_length = sizeof(client);
	int connfd = accept(sock, (struct sockaddr*)&client, &client_length);
	if(connfd < 0){
		printf("faile to connect, erron is: %d\n", errno);
	} else {
		char buffer[BUFFER_SIZE];
		memset(buffer, '\0', BUFFER_SIZE);
		while(recv(connfd, buffer, BUFFER_SIZE-1, 0) > 0) {	}
		close(connfd);
	}

	close(sock);
	return 0;
}
