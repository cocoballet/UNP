OBJ = 15.6_thread_pool_web.o 15.4_http_conn.o

edit : $(OBJ)
	g++ -o edit $(OBJ) -lpthread
15.6_thread_pool_web.o : 14.2_locker.h 15.3_thread_pool.h 15.4_http_conn.h
	g++ -c 15.6_thread_pool_web.cpp
15.4_http_conn.o : 15.4_http_conn.h
	g++ -c 15.4_http_conn.cpp

.PHONY : clean
clean : 
	rm edit $(OBJ)
