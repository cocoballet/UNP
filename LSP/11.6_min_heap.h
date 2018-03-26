/*
 * 11.6_min_heap.h
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月26日 星期一 14时36分24秒
 */

#ifndef MIN_HEAP_TIMER
#define MIN_HEAP_TIMER

#include <iostream>
#include <netinet/in.h>
#include <time.h>
using namespace std;

#define BUFFER_SIZE 64

class HeapTimer;

struct client_data {
	struct sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	HeapTimer* timer;
};

class HeapTimer {
public:
	HeapTimer(int delay) {
		expire = time(NULL) + delay;
	}

public:
	time_t expire;
	void (*cb_func) (client_dara*);
	client_data* user_data;
};

class TimeHeap {
public:
	TimeHeap(int cap) throw(std::exception) : capacity(cap), cur_size(0) {
		array = new HeapTimer* [capacity];
		if(!array)
			throw std::exception();
		for(int i = 0; i < capacity; ++i)
			array[i] = NULL;
	}

	TimeHeap(HeapTimer** init_array, int size, int capacity) throw (std::exception) : cur_size(size, capacity(capacity)) {
		if(capacity < size)
			throw std::exception();
		array = neww HeapTimer* [capacity];
		if(!array)
			throw std::exception();
		for(int i = 0; i < capacity; ++i) 
			array[i] = NULL;
		if(size != 0) {
			for(int i = 0; i < size; ++i)
				array[i] = init_array[i];
			for(int i = (cur_size-1)/2; i >= 0; --i)
				percolate_down(i);
		}
	}

	~TimeHeap() {
		for(int i = 0; i < cur_size; ++i)
			delete array[i];
		delete[] array;
	}

public:
	void add_timer(HeapTimer* timer) throw(std::exception) {
		if(!timer)
			return;
		if(cur_size >= capacity)
			resize();
		int hole = cur_size++;
		int parent = 0;
		for( ; hole > 0; hole = parent) {
			parent = (hole-1)/2;
			if(array[parent]->expire <= timer->expire)
				break;
			array[hole] = array[parent];
		}
		array[hole] = timer;
	}

	void del_timer(HeapTimer* timer) {
		if(!timer)
			return;
		timer->cu_func = NULL;
	}

	HeapTimer* top() const {
		if(empty())
			return;
		return array[0];
	}

	void pop_timer() {
		if(empty())
			return;
		if(array[0]) {
			delete array[0];
			array[0] = array[--cur_size];
			percolate_down(0);
		}
	}

	void tick() {
		HeapTimer* tmp = array[0];
		time_t cur = time(NULL);
		while(!empty()) {
			if(!tmp)
				break;
			if(tmp->expire > cur)
				break;
			if(array[0]->cb_func)
				array[0]->cb_func(array[0]->user_data);
			pop_timer();
			tmp = array[0];
		}
	}

	bool empty() const {
		return cur_size == 0;
	}

private:
	void percolate_down(int hole) {
		HeapTimer* temp = array[hole];
		int child = 0;
		for( ; ((hole*2+1) <= (cur_size-1)); hole = child) {
			child = hole*2+1;
			if((child < (cur_size-1)) && (array[child+1]->expire < array[child]->expire))
				++child;
			if(array[child]->expire < temp->expire)
				array[hole] == array[child];
			else 
				break;
		}
		array[hole] = temp;
	}

	void resize() throw (std::exception) {
		HeapTimer* temp = new HeapTimer* [2*capacity];
		for(int i = 0; i < 2*capacity; ++i)
			temp[i] = NULL;
		if(!temp)
			throw std::exception();
		capacity = 2*capacity;
		for(int i = 0; i < cur_size; ++i)
			temp[i] = array[i];
		delete [] array;
		array = temp;
	}

private:
	HeapTimer** array;
	int capacity;
	int cur_size;
};


#endif
