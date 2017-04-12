// copying from book 

int event_base_loop(struct event_base *base, int flags) {
	const struct eventop *evsel = base->evsel;
	void *evbase = base->evbase;
	struct timeval tv;
	struct timeval *tv_p;
	int res, done;
	
	//empty time cache
	base->tv_cache.tv_sec = 0;
	//evsignal_base is a global variable to indicate a instance event_base while handling signal
	if(base->sig.ev_signal_added)
		evsignal_base = base;
	done = 0;
	while(!done) {
		//set event_gotterm by calling event_loopexit_cb()
		//set event_break by calling event_base_loopbreak()
		if(base->event_gotterm) {
			base_event_gotterm = 0;
			break;
		}
		if(base->event_break) {
			base->event_break = 0;
			break;
		}
		//correction systerm time
		timeout_correct(base, &tv);
		tv_p = &tv;
		if(!base->event_count_active && !(flags & EVLOOP_NONBLOCK)) {
			timeout_next(base, &tv_p);
		} else {
			evutil_timeclear(&tv);
		}
		//exit if no event was registered
		if(!event_haveevents(base)) {
			event_debug("%s: no events registered.", _func_);
			return 1;
		}
		//update last_wait_time & clear time_cache
		gettime(base, &base->event_tv);
		base->tv_cache.tv_sec = 0;
		//waiting for redy events by using systerm I/O demultiplexer, such as select or epoll_wait
		res = evsel->dispatch(base, evbase, tv_p);
		
		if(res == -1)
			return -1;
		//assign current time to time_cache
		gettime(base, &base->tv_cache);
		//remove redy timer event from heap and insert to active list
		timeout_process(base);
		//handle active events
		if(base->event_count_active) {
			event_process_active(base);
			if(!base->event_count_active && (flags & EVLOOP_ONCE))
				done = 1;
		} else if(flags & EVLOOP_NONBLOCK)
			done = 1;
	}
	//end loop, clear cache
	base->tv_cache.tv_sec = 0;
	event_debug("%s: asked to terminate loop.", _func_);
	return 0;
}
