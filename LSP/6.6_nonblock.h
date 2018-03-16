/*
 * 6.6_nonblock.h
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月15日 星期四 09时32分55秒
 */

#include <iostream>
using namespace std;

int setnonblocking(int fd) {
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}
