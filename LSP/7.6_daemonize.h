/*
 * 7.6_daemonize.h
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月15日 星期四 11时51分50秒
 */

#include <iostream>
using namespace std;

bool daemonize() {
	pid_t pid = fork();
	if(pid < 0)
		return false;
	else if(pid > 0)
		exit(0);

	umask(0);

	pidd_t sid = setsid();
	if(sid < 0)
		return false;
	if((chdir("/")) < 0)
		return false;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FLIENO);

	open("dev/null", O_RDONLY);
	open("dev/null", O_RDWR);
	open("dev/null", O_RDWR);
	return true;
}
