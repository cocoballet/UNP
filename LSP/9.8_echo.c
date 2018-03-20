/*
 * 9.8_echo.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月20日 星期二 10时51分07秒
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENT_NUMBER 1024
#define TCP_BUFFER_SIZE 512
#define UDP_BUFFER_SIZE 1024

int setnonblock(int fd) {
	int old_opt = fcntl(fd, F_GETFL);
	int new_opt = old_opt | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_opt);
	return old_opt;
}

void addfd(int epollfd, int fd) {
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblock(fd);
}

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address port_number\n", argv[0]);
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int tcpfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(tcpfd >= 0);
	ret = bind(tcpfd, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);
	ret = listen(tcpfd, 5);
	assert(ret != -1);

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int udpfd = socket(PF_INET, SOCK_DGRAM, 0);
	assert(udpfd >= 0);
	ret = bind(udpfd, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);

	struct epoll_event events[MAX_EVENT_NUMBER];
	int epollfd = epoll_create(5);
	assert(epollfd != -1);
	addfd(epollfd, tcpfd);
	addfd(epollfd, udpfd);

	while(1) {
		int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		if(number < 0) {
			printf("epoll failure\n");
			break;
		}
		for(int i =0; i < number; i++) {
			int sockfd = events[i].data.fd;
			if(sockfd == tcpfd) {
				struct sockaddr_in client;
				socklen_t client_len = sizeof(client);
				int connfd = accept(tcpfd, (struct sockaddr*)&client, &client_len);
				addfd(epollfd, connfd);
			} else if(sockfd == udpfd) {
				char buf[UDP_BUFFER_SIZE];
				memset(buf, '\0', TCP_BUFFER_SIZE);
				struct sockaddr_in client;
				socklen_t client_len = sizeof(client);
				ret = recvfrom(udpfd, buf, UDP_BUFFER_SIZE-1, 0, (struct sockaddr*)&client, &client_len);
				if(ret > 0)
					sendto(udpfd, buf, UDP_BUFFER_SIZE-1, 0, (struct sockaddr*)&client, client_len);
			} else if(events[i].events & EPOLLIN) {
				char buf[TCP_BUFFER_SIZE];
				while(1) {
					memset(buf, '\0', TCP_BUFFER_SIZE);
					ret = recv(sockfd, buf, TCP_BUFFER_SIZE-1, 0);
					if(ret < 0) {
						if((errno == EAGAIN) || (errno == EWOULDBLOCK))
							break;
						close(sockfd);
						break;
					} else if(ret == 0)
						close(sockfd);
					else
						send(sockfd, buf, ret, 0);
				}
			} else
				printf("something else happened\n");
		}
	}
	close(tcpfd);
	return 0;
}