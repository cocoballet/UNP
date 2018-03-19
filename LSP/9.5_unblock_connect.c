/*
 * 9.5_unblock_connect.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月19日 星期一 14时58分27秒
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 1023

int setnonblock(int fd) {
	int old_opt = fcntl(fd, F_GETFL);
	int new_opt = old_opt | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_opt);
	return old_opt;
}

int nonblock_connect(const char* ip, int port, int time) {
	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	int fd_opt = setnonblock(sockfd);
	ret = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
	if(ret == 0) {
		printf("connect with server immediately\n");
		fcntl(sockfd, F_SETFL, fd_opt);
		return sockfd;
	} else if(errno != EINPROGRESS) {
		printf("unblock connect not support\n");
		return -1;
	}

	fd_set readfds;
	fd_set writefds;
	struct timeval timeout;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &writefds);
	timeout.tv_sec = time;
	timeout.tv_usec = 0;

	ret = select(sockfd+1, NULL, &writefds, NULL, &timeout);
	if(ret <= 0) {
		printf("connect timeout\n");
		close(sockfd);
		return -1;
	}
	if(!FD_ISSET(sockfd, &writefds)) {
		printf("no events on sockfd found\n");
		close(sockfd);
		return -2;
	}

	int error = 0;
	socklen_t len = sizeof(error);
	if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
		printf("get socket option failure\n");
		close(sockfd);
		return -1;
	}
	if(error != 0) {
		printf("connection failed after select with the error: %d\n", error);
		close(sockfd);
		return -1;
	}

	printf("connection ready after select with the socket: %d\n", sockfd);
	fcntl(sockfd, F_SETFL, fd_opt);
	return sockfd;
}

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address port_number\n", basename(argv[0]));
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	int sockfd = nonblock_connect(ip, port, 10);
	if(sockfd < 0)
		return 1;
	close(sockfd);
	return 0;
}
