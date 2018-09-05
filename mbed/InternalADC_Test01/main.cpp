#include "mbed.h"


AnalogIn Ain0(A0);

DigitalOut CheckPin1(D4);

int main()
{
    printf("\r\n InternalADC Test01\r\n");

    for (;;) {
		CheckPin1.write(1);
		uint16_t v = Ain0.read_u16();
		CheckPin1.write(0);
		printf("%d\r\n", v);
	}
}
