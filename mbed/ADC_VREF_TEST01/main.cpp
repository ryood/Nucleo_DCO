/*
 * ADC_VREF Test01
 *
 * 2018.09.18
 *
 */

#include "mbed.h"

#define INTERNAL_VOLTAGE  (1.22f)
#define VDIV_R1           (68.0f)   // 分圧用抵抗値1
#define VDIV_R2           (33.0f)   // 分圧用抵抗値2
#define VOLTAGE_DIVIDE    ( (VDIV_R1 + VDIV_R2) / VDIV_R2 )

Serial pc(USBTX, USBRX);

//AnalogIn Ain0(A0);
AnalogIn Ain0(PB_1);
AnalogIn VrefInt(ADC_VREF);

DigitalOut CheckPin1(D4);

int main()
{
	pc.baud(115200);
	pc.printf("\r\nADC_VREF Test01\r\n");
	pc.printf("%s %s\r\n", __DATE__, __TIME__);
	pc.printf("System Clock: %lu Hz\r\n\r\n", SystemCoreClock);
	
	float vref = VrefInt.read();
	float vrefFactor = INTERNAL_VOLTAGE / vref;
	printf("INTERNAL_VOLTAGE: %f\r\n", INTERNAL_VOLTAGE);
	printf("VD_R1           : %f\r\n", VDIV_R1);
	printf("VD_R2           : %f\r\n", VDIV_R2);
	printf("VOLTAGE_DIVIDE  : %f\r\n", VOLTAGE_DIVIDE);
	printf("vref            : %f\r\n", vref);
	printf("vrefFactor      : %f\r\n", vrefFactor);

	printf("\r\nvref,\tvrefFactor,\tread,\tafter div,\tbefore div\r\n");
    for (;;) {
		CheckPin1.write(1);
		vref = VrefInt.read();
		vrefFactor = INTERNAL_VOLTAGE / vref;
		float v = Ain0.read();
		CheckPin1.write(0);
		printf("%5.3f,\t%5.3f,\t%5.3f,\t%5.3f V,\t%6.3f V\r\n", 
			vref, vrefFactor, v, v * vrefFactor, v * vrefFactor * VOLTAGE_DIVIDE);
		wait(1.0);
	}
}
