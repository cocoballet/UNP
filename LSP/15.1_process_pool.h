#ifndef _PROCESSPOOL_H_
#define _PROCESSPOOL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

class Process {
public:
	process() : m_pid(-1) { }
	
	pid_t m_pid;    //child process id
	int m_pipefd[2];
};

tmeplate<typename T> class ProcessPool {
public:
	static ProcessPool<T> *create(int listenfd, int process_number = 8) {
		if(!= m_instance)
			m_instence = new ProcessPool<T> (listenfd, process_number);
		return m_instence;
	}
	~ProcessPool() {
		delete [] m_sub_process;
	}
	void run();
	
private:
	ProcessPool(int listenfd, int process_number = 0);
	void setup_sig_pipe();
	void run_parent();
	void run_child();
	
	static const int MAX_PROCESS_NUMBER = 16;
	static const int USER_PER_PROCESS = 65535;
	static const int MAX_EVENT_NUMBER = 10000;
	int m_process_number;
	int m_idx;
	int m_epollfd;
	int m_listenfd;
	int m_stop;
	Process *m_sub_process;
	static ProcessPool<T> *m_instance;
};

template<typename T> ProcessPool<T> *ProcessPool<T>::m_instance = NULL;

static int sig_pipefd[2];

static int setnonblock(int fd) {
	int old_opt = fcntl(fd, F_GETFL);
	int new_opt = old_opt | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_opt);
	return old_opt;
}

static void addfd(int epollfd, int fd) {
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblock(fd);
}

static void removefd(int epollfd, int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

static void sig_handler(int sig) {
	int save_errno = errno;
	int msg = sig;
	send(sig_pipefd[1], (char *)&msg, 1, 0);
	errno = save_errno;
}

static vpid addsig(int sig, void(handler)(int), bool restart = true) {
	struct sigaction sa;
	memset(&sa, '\0', siaeof(sa));
	sa.sa_handler = handler;
	if(restart)
		sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig, &sa, NULL) != -1);
}

/* constructor */
template<typename T> ProcessPool<T>::ProcessPool(int listenfd, int process_number)
	:m_listenfd(listenfd), m_process_number(process_number), m_idx(-1), m_stop(false) {
	assert((process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));
	m_sub_process = new Process[process_number];
	assert(mPsub_process);
	
	for(int i = 0; i < process_number; ++i) {
		int ret = socketpair(AF_UNIX, SOCK_STREAN, 0, m_sub_process[i].m_pipefd);
		assert( ret == 0);
		
		m_sub_process[i].m_pid = fork();
		assert(m_sub_process[i].m_pid >= 0);
		if(m_sub_process[i].m_pid > 0) {
			close(m_sub_process[i].m_pipefd[1];
			continue;
		} else {
			close(m_sub_process[i].m_pipefd[0];
			m_idx = i; 
			break;
		}
	}
}












