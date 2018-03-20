/*
 * 9.7_chat_server.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月19日 星期一 15时39分53秒
 */

//#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>

#define USER_LIMIT 5
#define FD_LIMIT 65535
#define BUFFER_SIZE 64

struct client_data {
	struct sockaddr_in address;
	char* write_buf;
	char buf[BUFFER_SIZE];
};

int setnonblock(int fd) {
	int old_opt = fcntl(fd, F_GETFL);
	int new_opt = old_opt | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_opt);
	return old_opt;
}

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address port_number\n", argv[0]);
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	int ret = 0;
	struct sockaddr_in server;
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &server.sin_addr);
	server.sin_port = htons(port);

	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd >= 0);
	ret = bind(listenfd, (struct sockaddr*)&server, sizeof(server));
	assert(ret != -1);
	ret = listen(listenfd, 5);
	assert(ret != -1);

	client_data* users = new client_data[FD_LIMIT];
	struct pollfd fds[USER_LIMIT+1];
	int user_counter = 0;
	for(int i = 1; i <= USER_LIMIT; ++i) {
		fds[i].fd = -1;
		fds[i].events = 0;
	}
	fds[0].fd = listenfd;
	fds[0].events = POLLIN | POLLERR;
	fds[0].revents = 0;

	while(1) {
		ret = poll(fds, user_counter+1, -1);
		if(ret < 0) {
			printf("poll failure\n");
			break;
		}
		for(int i = 0; i < user_counter+1; ++i) {
			if((fds[i].fd == listenfd) && (fds[i].revents & POLLIN)) {
				struct sockaddr_in client;
				socklen_t client_len = sizeof(client);
				int connfd = accept(listenfd, (struct sockaddr*)&client, &client_len);
				if(connfd < 0) {
					printf("errno is: %d\n", errno);
					continue;
				}
				if(user_counter >= USER_LIMIT) {
					const char* info = "too many users\n";
					printf("%s", info);
					send(connfd, info, strlen(info), 0);
					close(connfd);
					continue;
				}
				user_counter++;
				users[connfd].address = client;
				setnonblock(connfd);
				fds[user_counter].fd = connfd;
				fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
				fds[user_counter].revents = 0;
				printf("new user arrvil, now %d users in total\n", user_counter);
			} else if(fds[i].revents & POLLRDHUP) {
				users[fds[i].fd] = users[fds[user_counter].fd];
				close(fds[i].fd);
				fds[i] = fds[user_counter];
				i--;
				user_counter--;
				printf("a client left\n");
			} else if(fds[i].revents & POLLIN) {
				int connfd = fds[i].fd;
				memset(users[connfd].buf, '\0', BUFFER_SIZE);
				ret = recv(connfd, users[connfd].buf, BUFFER_SIZE-1, 0);
				printf("get %d bytes of client data %s from user %d\n", ret, users[connfd].buf, connfd);
				if(ret < 0) {
					if(errno != EAGAIN) {
						close(connfd);
						users[fds[i].fd] = users[fds[user_counter].fd];
						fds[i] = fds[user_counter];
						i--;
						user_counter--;
					}
				} else if(ret == 0) {

				} else {
					for(int j = 1; j <= user_counter; ++j) {
						if(fds[j].fd == connfd)
							continue;
						fds[j].events |= ~POLLIN;
						fds[j].events |= POLLOUT;
						users[fds[j].fd].write_buf = users[connfd].buf;
					}
				}
			} else if(fds[i].revents & POLLOUT) {
				int connfd = fds[i].fd;
				if(!users[connfd].write_buf)
					continue;
				ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
				users[connfd].write_buf = NULL;
				fds[i].events |= ~POLLOUT;
				fds[i].events |= POLLIN;
			}
		}
	}

	delete [] users;
	close(listenfd);
	return 0;
}
