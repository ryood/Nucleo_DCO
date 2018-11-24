/*
   RotaryEncoder Test01

   2018.11.24

*/
#include "mbed.h"
#include "RotaryEncoder.h"

#define ROTENC_INTERVAL  (2000)
#define MIN_VALUE  (0)
#define MAX_VALUE  (8)
#define DEF_VALUE  (0)

Serial pc(USBTX, USBRX);

RotaryEncoder RotEnc1(PC_8,  PC_9,  MIN_VALUE, MAX_VALUE, DEF_VALUE);
RotaryEncoder RotEnc2(PC_10, PC_11, MIN_VALUE, MAX_VALUE, DEF_VALUE);
RotaryEncoder RotEnc3(PC_12, PD_2,  MIN_VALUE, MAX_VALUE, DEF_VALUE);
RotaryEncoder RotEnc4(PG_2,  PG_3,  MIN_VALUE, MAX_VALUE, DEF_VALUE);
RotaryEncoder RotEnc5(PD_7,  PD_6,  MIN_VALUE, MAX_VALUE, DEF_VALUE);
RotaryEncoder RotEnc6(PD_5,  PD_4,  MIN_VALUE, MAX_VALUE, DEF_VALUE);

int main()
{
	pc.baud(115200);
	pc.printf("\r\nRotary Encoder Test02\r\n");
	pc.printf("%s %s\r\n", __DATE__, __TIME__);
	
	RotEnc1.setInterval(ROTENC_INTERVAL);
	RotEnc2.setInterval(ROTENC_INTERVAL);
	RotEnc3.setInterval(ROTENC_INTERVAL);
	RotEnc4.setInterval(ROTENC_INTERVAL);
	RotEnc5.setInterval(ROTENC_INTERVAL);
	RotEnc6.setInterval(ROTENC_INTERVAL);
	
	while(1) {
		int v1 = RotEnc1.getVal();
		int v2 = RotEnc2.getVal();
		int v3 = RotEnc3.getVal();
		int v4 = RotEnc4.getVal();
		int v5 = RotEnc5.getVal();
		int v6 = RotEnc6.getVal();
		printf("%d,\t%d,\t%d,\t%d,\t%d,\t%d\r\n", v1, v2, v3, v4, v5, v6);
	}
}
