/*
 * 5.1_endian.c
 * Author: Andrea
 * Note: 
 * Created Time: 2018年03月12日 星期一 16时44分40秒
 */

#include<stdio.h>

void byteorder() {
	union {
		short value;
		char union_byte[sizeof(short)];
	} test;
	test.value = 0x0102;
	if((test.union_byte[0] == 1)&&(test.union_byte[1] == 2)) {
		printf("big endian\n");
	} else if((test.union_byte[0] == 2)&&(test.union_byte[1] == 1)) {
		printf("little endian\n");
	} else {
		printf("unknow\n");
	}
}

int main() {
	byteorder();
	return 0;
}

