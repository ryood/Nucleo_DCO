#include "mbed.h"
#include "AverageAnalogIn.h"

#define TITLE_STR1  ("AverageAnalogIn Test01")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define ADC_NUM  (10)

AverageAnalogIn Adc[ADC_NUM] = {
	AverageAnalogIn(PA_3),
	AverageAnalogIn(PC_0),
	AverageAnalogIn(PC_3),
	AverageAnalogIn(PF_3),
	AverageAnalogIn(PF_5),
	AverageAnalogIn(PF_10),
	AverageAnalogIn(PA_0),
	AverageAnalogIn(PF_8),
	AverageAnalogIn(PF_7),
	AverageAnalogIn(PF_9),
};

Serial pc(USBTX, USBRX);

int main()
{
	pc.baud(115200);
	pc.printf("\r\n%s\r\n", TITLE_STR1);
	pc.printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);
	
	while (1) {
		float v[ADC_NUM];
		for (int i = 0; i < ADC_NUM; i++) {
			v[i] = Adc[i].read();
		}
		
		for (int i = 0; i < ADC_NUM; i++) {
			printf("%1.3f\t", v[i]);
		}
		printf("\r\n");
		
		wait(0.1);
	}
}
