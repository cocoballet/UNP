/*
 * 5.3_backlog.c
 * Author: Andrea
 * Note: test backlog
 * Created Time: 2018年03月13日 星期二 14时04分53秒
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

static bool stop = false;
/* SIGTERM handler, */
static void handle_term(int sig) {
	stop = true;
}

int main(int argc, char* argv[]) {
	signal(SIGTERM, handle_term);
	if(argc <= 3) {
		printf("usage: %s ip_adress, port_number, backlog\n", basename(argv[0]));
		return 1;
	}
	
	const char* ip = argv[1];
	int port = atoi(argv[2]);
	int backlog = atoi(argv[3]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(sock >= 0);

	/* create a IPv4 socket address */
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
	assert(ret >= 0);

	ret = listen(sock, backlog);
	assert(ret >= 0);

	/* wait for link until SIGTERM */
	while(!stop) {
		sleep(1);
	}

	/* close socket */
	close(sock);
	return 0;
}
