#include "mbed.h"

#define AIN_NUM  (10)

Serial pc(USBTX, USBRX);

AnalogIn Ain[AIN_NUM] = {
	AnalogIn(PA_3),
	AnalogIn(PC_0),
	AnalogIn(PC_3),
	AnalogIn(PF_3),
	AnalogIn(PF_5),
	AnalogIn(PF_10),
	AnalogIn(PA_7),
	AnalogIn(PF_8),
	AnalogIn(PF_7),
	AnalogIn(PF_9)
};

int main()
{
	pc.baud(115200);
    pc.printf("\r\nPOT Panel Test02\r\n%s %s\r\n", __DATE__, __TIME__);

    while (1) {
		for (int i = 0; i < AIN_NUM; i++) {
			uint16_t v = Ain[i].read_u16();
			pc.printf("%5d\t", v);
		}
		pc.printf("\r\n");
	}
}
