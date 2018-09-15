/*
   RotaryEncoder Test01

   2018.09.15

*/
#include "mbed.h"
#include "RotaryEncoder.h"

Serial pc(USBTX, USBRX);

RotaryEncoder RotEnc1(PC_8,  PC_9,  0, 8, 0);
RotaryEncoder RotEnc2(PC_10, PC_11, 0, 8, 0);

int main()
{
	pc.baud(115200);
	pc.printf("\r\nRotary Encoder Test01\r\n");
	pc.printf("%s %s\r\n", __DATE__, __TIME__);
	
	RotEnc1.setInterval(2000);
	RotEnc2.setInterval(2000);
	
	while(1) {
		int v1 = RotEnc1.getVal();
		int v2 = RotEnc2.getVal();
		printf("%d\t%d\r\n", v1, v2);
	}
}
