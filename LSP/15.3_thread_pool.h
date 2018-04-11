/*
 * 15.5_thread_pool.h
 * Author: Andrea
 * Note: 
 * Created Time: 2018年04月10日 星期二 13时48分56秒
 */

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>

#include "14.2_locker.h"

template<typename T> class ThreadPool {
public:
	ThreadPool(int thread_number = 8, int max_request = 10000);
	~ThreadPool();

	bool append(T* request);
private:
	static void* worker(void* arg);
	void run();

	int m_thread_number;
	int m_max_request;
	pthread_t* m_threads;
	std::list<T*> m_workqueue;
	Locker m_queuelock;
	Sem m_queuestat;
	bool m_stop;
};

template<typename T> ThreadPool<T>::ThreadPool(int thread_number, int max_request) : m_thread_number(thread_number), m_max_request(max_request), m_stop(false), m_threads(NULL) {
	if((thread_number <= 0) || (max_request <= 0))
		throw std::exception();
	m_threads = new pthread_t[m_thread_number];
	if(!m_threads)
		throw std::exception();
	for(int i = 0; i < thread_number; ++i) {
		printf("The %dth thread created\n", i);
		if(pthread_create(m_threads+i, NULL, worker, this) != 0) {
			delete [] m_threads;
			throw std::exception();
		}
		if(pthread_detach(m_threads[i])) {
			delete [] m_threads;
			throw std::exception();
		}
	}
}

template<typename T> ThreadPool<T>::~ThreadPool() {
	delete [] m_threads;
	m_stop = true;
}

template<typename T> bool ThreadPool<T>::append(T* request) {
	m_queuelock.lock();
	if(m_workqueue.size() > m_max_request) {
		m_queuelock.unlock();
		return false;
	}
	m_workqueue.push_back(request);
	m_queuelock.unlock();
	m_queuestat.post();
	return true;
}

template<typename T> void* ThreadPool<T>::worker(void* arg) {
	ThreadPool* pool = (ThreadPool*)arg;
	pool->run();
	return pool;
}

template<typename T> void ThreadPool<T>::run() {
	while(!m_stop) {
		m_queuestat.wait();
		m_queuelock.lock();
		if(m_workqueue.empty()) {
			m_queuelock.unlock();
			continue;
		}
		T* request = m_workqueue.front();
		m_workqueue.pop_front();
		m_queuelock.unlock();
		if(!request)
			continue;
		request->process();
	}
}

#endif
