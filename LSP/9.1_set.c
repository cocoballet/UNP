/*
 * 9.1_set.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月16日 星期五 10时31分44秒
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
	if(argc <= 2) {
		printf("usage: %s ip_address port_number\n", basename(argv[0]));
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);
	int ret = 0;

	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd >= 0);
	ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);
	ret = listen(listenfd, 5);
	assert(ret != -1);

	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int connfd = accept(listenfd, (struct sockaddr*)&client, &client_len);
	if(connfd < 0) {
		printf("connection failure: %d", errno);
		close(listenfd);
	}

	char buf[1024];
	fd_set read_fds;
	fd_set exception_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&exception_fds);

	while(1) {
		memset(buf, '\0', sizeof(buf));
		FD_SET(connfd, &read_fds);
		FD_SET(connfd, &exception_fds);
		ret = select(connfd+1, &read_fds, NULL, &exception_fds, NULL);
		if(ret < 0) {
			printf("selection failure\n");
			break;
		}
		if(FD_ISSET(connfd, &read_fds)) {
			ret = recv(connfd, buf, sizeof(buf)-1, 0);
			if(ret <= 0)
				break;
			printf("get %d bytes of normal data: %s\n", ret, buf);
		} else if(FD_ISSET(connfd, &exception_fds)) {
			ret = recv(connfd, buf, sizeof(buf)-1, MSG_OOB);
			if(ret < 0)
				break;
			printf("get %d bytes of OOB data: %s\n", ret, buf);
		}
	}
	close(connfd);
	close(listenfd);
	return 0;
}
