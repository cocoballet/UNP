/*
 * 5.10_sendBuff.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月14日 星期三 09时29分32秒
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address, port_number, send_buffer_size\n", basename(argv[0]));
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &server_address.sin_addr);
	server_address.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(sock >= 0);

	int sendbuff = atoi(argv[3]);
	int len = sizeof(sendbuff);

	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));
	getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuff, &len);
	printf("tcp_buff size after setting is %d\n", sendbuff);

	if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) != -1) {
		char buffer[BUFFER_SIZE];
		memset(buffer, 'a', BUFFER_SIZE);
		send(sock, buffer, BUFFER_SIZE, 0);
	}

	close(sock);
	return 0;
}
