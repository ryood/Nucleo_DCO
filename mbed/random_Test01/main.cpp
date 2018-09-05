/*
 * random()の処理時間を計測
 * 
 * 2018.09.04
 * 
 */
#include "mbed.h"

#define LOOP_N  (1000)

int main()
{
	printf("\r\nrandom() Test\r\n");
	printf("LOOP_N\t%d\r\n", LOOP_N);
	
	Timer t;
	t.start();
	for (int i = 0; i < LOOP_N; i++) {
		long r = random();
	}
	int elapse = t.read_us();
	printf("Total: %d us\r\n", elapse);
	printf("1/op: %f us\r\n", (float)elapse / LOOP_N);
	printf("freq: %f MHz\r\n", (float)LOOP_N / elapse);
	printf("end\r\n");
}
