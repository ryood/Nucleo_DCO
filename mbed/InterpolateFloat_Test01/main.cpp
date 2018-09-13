/*
 * InterpolateFloat Test01
 *
 * 2018.09.13
 *
 */

#include "mbed.h"


#define TITLE_STR1  ("InterpolateFloat Test01")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define DIVISION	(8)

//Ticker ticker;
Serial pc(USBTX, USBRX);
#include "InterpolateFloat.h"

int main()
{
	pc.baud(115200);
	pc.printf("\r\n%s\r\n", TITLE_STR1);
	pc.printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);

	InterpolateFloat interpolate(0, DIVISION);
	
	interpolate.setNext(255);
	for (int i = 0; i < DIVISION; i++) {
		float v = interpolate.get();
		pc.printf("%f\t%d\r\n", v, (int)v);
	}
	pc.printf("\r\nend\r\n");
}
