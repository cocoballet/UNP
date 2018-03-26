/*
 * 11.5_time_wheel.h
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月26日 星期一 13时44分38秒
 */

#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <time.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFFER_SIZE 64

class TwTimer;

struct client_data {
	struct sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	TwTimer* timer;
};

class TwTimer {
public:
	TwTimer(int rot, int ts) : next(NULL), prev(NULL), rotation(rot), time_slot(ts) {	}

public:
	int rotation;
	int time_slot;
	void (*cb_func) (client_data*);
	client_data* user_data;
	TwTimer* next;
	TwTimer* prev;
};

class TimeWheel {
public:
	TimeWheel() : cur_slot(0) {
		for(int i = 0; i < N; ++i)
			slots[i] = NULL;
	}
	
	~TimeWheel() {
		for(int i = 0; i < N; ++i) {
			TwTimer* tmp = slots[i];
			while(tmp) {
				slots[i] = tmp->next;
				delete tmp;
				tmp = slots[i];
			}
		}
	}

	TwTimwe* add_timer(int timeout) {
		if(timeout < 0)
			return;
		int ticks = 0;
		if(timeout < SI)
			ticks = 1;
		else
			ticks = timeout / SI;
		int rotation = ticks / N;
		int ts = (cur_slot + (ticks % N)) % N;
		TwTimer* timer = new TwTimer(rotation, ts);
		if(!slots[ts]) {
			printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot);
			slots[ts] = timer;
		} else {
			timer->next = slots[ts];
			slots[ts]->prev = timer;
			slots[ts] = timer;
		}
		return timer;
	}

	void del_timer(TwTimer* timer) {
		if(!timer)
			return;
		int ts = timer->time_slot;
		if(timer == slots[ts]) {
			slots[ts] = slots[ts]->next;
			if(slots[ts])
				slots[ts]->prev = NULL;
			delete timer;
		} else {
			timer->prev->next = timer->next;
			if(timer->next)
				timer->next->prev - timer->prev;
			delete timer;
		}
	}

	void tick() {
		TwTimer* tmp = slots[cur_slot];
		printf("current slot is %d\n", cur_slot);
		while(tmp) {
			printf("tick time\n");
			if(tmp->rotation > 0) {
				tmp->rotation--;
				tmp = tmp->next;
			} else {
				tmp->cb_func(tmp->user_data);
				if(tmp == slots[cur_slot]) {
					printf("delete header in cur_slot\n");
					slots[cur_slot] = tmp->next;
					delete tmp;
					if(slots[cur_slot])
						slots[cur_slot]->prev = NULL;
					tmp = slots[cur_slot];
				} else {
					tmp->prev->next = tmp->next;
					if(tmp->next)
						tmp->next->prev = tmp->prev;
					TwTimer* tmp2 = tmp->next;
					delete tmp;
					tmp = tmp2;
				}
			}
		}
		cur_slot = ++cur_slot % N;
	}

private:
	static const int N = 60;
	static const int SI = 1;
	TwTimer* slots[N];
	int cur_slot;
};

#endif
