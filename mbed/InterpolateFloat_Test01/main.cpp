/*
 * InterpolateFloat Test01
 *
 * 2018.09.13
 *
 */

#include "mbed.h"
#include "InterpolateFloat.h"

#define TITLE_STR1  ("InterpolateFloat Test01")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define ADC_NUM	(10)
#define INTERPOLATE_DIVISION	(8)
#define PRUNING_FACTOR	(1.05f)

AnalogIn Adc[ADC_NUM] = {
	AnalogIn(PA_3),
	AnalogIn(PC_0),
	AnalogIn(PC_3),
	AnalogIn(PF_3),
	AnalogIn(PF_5),
	AnalogIn(PF_10),
	AnalogIn(PA_0),
	AnalogIn(PF_8),
	AnalogIn(PF_7),
	AnalogIn(PF_9),
};

Ticker ticker;
Serial pc(USBTX, USBRX);

InterpolateFloat interpolate[ADC_NUM] = {
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
	InterpolateFloat(0, INTERPOLATE_DIVISION),
};

void update()
{
	for (int i = 0; i < ADC_NUM; i++) {
		float v = interpolate[i].get();
		printf("%1.3f,\t", v);
	}
	printf("\r\n");
}

inline float pruning(float v)
{
	float ret = ((v - 0.5f) * PRUNING_FACTOR) + 0.5f;
	if (ret < 0.0f) {
		ret = 0.0f;
	} else if (ret > 1.0f) {
		ret = 1.0f;
	}
	return ret;
}
	
int main()
{
	pc.baud(115200);
	pc.printf("\r\n%s\r\n", TITLE_STR1);
	pc.printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);

	for (int i = 0; i < ADC_NUM; i++) {
		float v = Adc[i].read();
		interpolate[i].setNext(v);
	}
	
	ticker.attach_us(&update, 10000);
	
	while (1) {
		for (int i = 0; i < ADC_NUM; i++) {
			float v = pruning(Adc[i].read());
			interpolate[i].setNext(v);
		}
	}
}
