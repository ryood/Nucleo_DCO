#include "mbed.h"

#define AIN_NUM  (10)

AnalogIn Ain[AIN_NUM] = {
	AnalogIn(A0),
	AnalogIn(A1),
	AnalogIn(A2),
	AnalogIn(A3),
	AnalogIn(A4),
	AnalogIn(A5),
	AnalogIn(D13),
	AnalogIn(D12),
	AnalogIn(D11),
	AnalogIn(PC_4)
};

int main()
{
    printf("\r\nPOT Panel Test01\r\n%s %s", __DATE__, __TIME__);

    while (1) {
		for (int i = 0; i < AIN_NUM; i++) {
			uint16_t v = Ain[i].read_u16();
			printf("%5d\t", v);
		}
		printf("\r\n");
	}
}
