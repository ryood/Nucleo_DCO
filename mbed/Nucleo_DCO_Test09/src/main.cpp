/*
   Nucleo DCO Test09

   u8g2-mbed + DDS(3OSC) + InternalADC + InterruptIn
   64k wavetable
   
   2018.09.08

*/
#include "mbed.h"
#include "u8g2.h"
#include <math.h>

#include "wavetable_12bit_64k.h"
#define COUNT_OF_ENTRIES  (65536)

#define PIN_CHECK   (1)
#define UART_TRACE  (1)
#define TITLE_STR1  ("DCO Test09")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define OSC_NUM              (3)
#define FREQUENCY_RANGE_MAX  (6)
#define REF_CLOCK            (100000)  // 100kHz

#define DEBOUNCE_DELAY  (10000)  // usec

// Pin Assign
AnalogOut Dac1(PA_4);
AnalogOut Dac2(PA_5);

AnalogIn Adc1(PA_3);
AnalogIn Adc2(PC_0);
AnalogIn Adc3(PC_3);
AnalogIn Adc4(PF_3);
AnalogIn Adc5(PF_5);
AnalogIn Adc6(PF_10);
AnalogIn Adc7(PA_7);
AnalogIn Adc8(PF_8);
AnalogIn Adc9(PF_7);
AnalogIn Adc10(PF_9);
AnalogIn Adc11(PB_1);
AnalogIn Adc12(PC_2);

// Interrupt: cannot use same pin number
//  not use PE_10
InterruptIn Button0(PB_11, PullUp);
InterruptIn Button1(PB_10, PullUp);
InterruptIn Button2(PE_15, PullUp);
InterruptIn Button3(PE_14, PullUp);
InterruptIn Button4(PE_12, PullUp);
InterruptIn Button5(PE_7,  PullUp);
InterruptIn Button6(PE_8,  PullUp);
InterruptIn Button7(PE_0,  PullUp);

#if (PIN_CHECK)
DigitalOut CheckPin1(D4);
DigitalOut CheckPin2(D5);
#endif

// u8g2
uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t U8g2Handler;

// Interruput
Ticker ticker;

// UART
#if (UART_TRACE)
Serial pc(USBTX, USBRX);
#endif

// Parameter
enum {
	WS_SIN,
	WS_TRI,
	WS_SAWUP,
	WS_SAWDOWN,
	WS_SQR,
	WS_NOISE,
	WS_MAX
};

double drate[OSC_NUM]          // output rate (Hz)
	= { 1000.0, 1000.0, 1000.0 };
float phase[OSC_NUM]           // output phase (0.0 ~ 1.0)
	= { 0.0f, 0.0f, 0.0f };
int pulseWidth[OSC_NUM]        // output pulseWidth (0 ~ COUNT_OF_ENTRIES)
	= { COUNT_OF_ENTRIES/2, COUNT_OF_ENTRIES/2, COUNT_OF_ENTRIES/2 };
float amplitude[OSC_NUM]       // output amplitude (0.0 ~ 1.0)
	= { 0.3f, 0.3f, 0.3f };
int waveShape[OSC_NUM]
	= { WS_SAWUP, WS_SAWUP, WS_SAWUP };
int frequencyRange[OSC_NUM]
	= { 1, 1, 1 };

// DDS
volatile uint32_t phaccu[OSC_NUM];
volatile uint32_t tword_m[OSC_NUM];

// Debounce
Ticker debouncer0;
Ticker debouncer1;
Ticker debouncer2;
Ticker debouncer3;
Ticker debouncer4;
Ticker debouncer5;
Ticker debouncer6;
Ticker debouncer7;

volatile bool isButtonPushed0 = false;
volatile bool isButtonPushed1 = false;
volatile bool isButtonPushed2 = false;
volatile bool isButtonPushed3 = false;
volatile bool isButtonPushed4 = false;
volatile bool isButtonPushed5 = false;
volatile bool isButtonPushed6 = false;
volatile bool isButtonPushed7 = false;

//-------------------------------------------------------------------------------------------------
// Interrupt Service Routine
//

// param
//   channel: 1 or 2
//   val: 0 .. 4095
void internalDacWrite(int ch, uint16_t val)
{
    //uint16_t v = val << 4;
    // avoid distortion of the built-in DAC
    uint16_t v = (val + 256) * 14;
    
    switch(ch) {
    case 1:
        Dac1.write_u16(v);
        break;
    case 2:
        Dac2.write_u16(v);
        break;
    }
}

void update()
{
#if (PIN_CHECK)
    CheckPin1.write(1);
#endif

	int32_t sum = 0;
	for (int i = 0; i < OSC_NUM; i++) {

		phaccu[i] = phaccu[i] + tword_m[i];

		uint32_t phi = UINT32_MAX * phase[i];
		uint16_t idx = (phaccu[i] + phi) >> 16;  // use upper 16 bits

		uint16_t v = 0;
		switch (waveShape[i]) {
		case WS_SIN:
			v = sin_12bit_64k[idx];
			break;
		case WS_TRI:
			v = tri_12bit_64k[idx];
			break;
		case WS_SQR:
			if (idx < pulseWidth[i]) {
				v = 4095;
			} else {
				v = 0;
			}
			break;
		case WS_SAWUP:
			v = idx >> 4;
			break;
		case WS_SAWDOWN:
			v = 4095 - (idx >> 4);
			break;
		case WS_NOISE:
			v = rand() % 4095;
			break;
		}

		// Mix
		sum += v * amplitude[i];
	}
	
	// sum = sum / OSC_NUM;
	
	// limiter
	if (sum < 0) {
		sum = 0;
	}
	else if (sum > 4095) {
		sum = 4095;
	}

	internalDacWrite(1, (uint16_t)sum);

#if (PIN_CHECK)
    CheckPin1.write(0);
#endif
}

//-------------------------------------------------------------------------------------------------
// Debounce
//

void debounce0() { if (Button0.read() == 0) { isButtonPushed0 = true; } debouncer0.detach(); }
void debounce1() { if (Button1.read() == 0) { isButtonPushed1 = true; } debouncer1.detach(); }
void debounce2() { if (Button2.read() == 0) { isButtonPushed2 = true; } debouncer2.detach(); }
void debounce3() { if (Button3.read() == 0) { isButtonPushed3 = true; } debouncer3.detach(); }
void debounce4() { if (Button4.read() == 0) { isButtonPushed4 = true; } debouncer4.detach(); }
void debounce5() { if (Button5.read() == 0) { isButtonPushed5 = true; } debouncer5.detach(); }
void debounce6() { if (Button6.read() == 0) { isButtonPushed6 = true; } debouncer6.detach(); }
void debounce7() { if (Button7.read() == 0) { isButtonPushed7 = true; } debouncer7.detach(); }

void interruptHandler0() { debouncer0.attach_us(&debounce0, DEBOUNCE_DELAY); }
void interruptHandler1() { debouncer1.attach_us(&debounce1, DEBOUNCE_DELAY); }
void interruptHandler2() { debouncer2.attach_us(&debounce2, DEBOUNCE_DELAY); }
void interruptHandler3() { debouncer3.attach_us(&debounce3, DEBOUNCE_DELAY); }
void interruptHandler4() { debouncer4.attach_us(&debounce4, DEBOUNCE_DELAY); }
void interruptHandler5() { debouncer5.attach_us(&debounce5, DEBOUNCE_DELAY); }
void interruptHandler6() { debouncer6.attach_us(&debounce6, DEBOUNCE_DELAY); }
void interruptHandler7() { debouncer7.attach_us(&debounce7, DEBOUNCE_DELAY); }

//-------------------------------------------------------------------------------------------------
// Main routine
//

void u8g2Initialize()
{
	//u8g2_Setup_ssd1306_i2c_128x64_noname_f(&myScreen, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(&U8g2Handler, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_InitDisplay(&U8g2Handler);
	u8g2_SetPowerSave(&U8g2Handler, 0);
	u8g2_ClearBuffer(&U8g2Handler);
}

void debouncerInitialize()
{
	Button0.fall(&interruptHandler0);
	Button1.fall(&interruptHandler1);
	Button2.fall(&interruptHandler2);
	Button3.fall(&interruptHandler3);
	Button4.fall(&interruptHandler4);
	Button5.fall(&interruptHandler5);
	Button6.fall(&interruptHandler6);
	Button7.fall(&interruptHandler7);
}

void readAdcParameters()
{
	// OSC1
	drate[0]      = Adc1.read() * 200.0 + 10.0;
	phase[0]      = Adc2.read();
	pulseWidth[0] = Adc3.read() * COUNT_OF_ENTRIES;
	amplitude[0]  = (Adc4.read_u16() >> 12) / 15.0f;

	// OSC2
	drate[1]      = Adc5.read() * 200.0 + 10.0;
	phase[1]      = Adc6.read();
	pulseWidth[1] = Adc7.read() * COUNT_OF_ENTRIES;
	amplitude[1]  = (Adc8.read_u16() >> 12) / 15.0f;

	// OSC3
	drate[2]      = Adc9.read() * 200.0 + 10.0;
	phase[2]      = Adc10.read();
	pulseWidth[2] = Adc11.read() * COUNT_OF_ENTRIES;
	amplitude[2]  = (Adc12.read_u16() >> 12) / 15.0f;
}

void readButtonParameters()
{
	// Wave shape
	if (isButtonPushed0) {
		waveShape[0]++;
		if (waveShape[0] >= WS_MAX) {
			waveShape[0] = 0;
		}
		isButtonPushed0 = false;
	}
	if (isButtonPushed1) {
		waveShape[1]++;
		if (waveShape[1] >= WS_MAX) {
			waveShape[1] = 0;
		}
		isButtonPushed1 = false;
	}
	if (isButtonPushed2) {
		waveShape[2]++;
		if (waveShape[2] >= WS_MAX) {
			waveShape[2] = 0;
		}
		isButtonPushed2 = false;
	}
	
	// Frequency range
	if (isButtonPushed3) {
		frequencyRange[0]++;
		if (frequencyRange[0] >= FREQUENCY_RANGE_MAX) {
			frequencyRange[0] = 0;
		}
		isButtonPushed3 = false;
	}
	if (isButtonPushed4) {
		frequencyRange[1]++;
		if (frequencyRange[1] >= FREQUENCY_RANGE_MAX) {
			frequencyRange[1] = 0;
		}
		isButtonPushed4 = false;
	}
	if (isButtonPushed5) {
		frequencyRange[2]++;
		if (frequencyRange[2] >= FREQUENCY_RANGE_MAX) {
			frequencyRange[2] = 0;
		}
		isButtonPushed5 = false;
	}
}

int main()
{
#if (UART_TRACE)
	pc.baud(115200);
	pc.printf("\r\n%s\r\n", TITLE_STR1);
	pc.printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);
	pc.printf("UINT32_MAX: %lu\r\n", UINT32_MAX);
	pc.printf("System Clock: %lu Hz\r\n", SystemCoreClock);
	pc.printf("CLOCKS_PER_SEC: %d\r\n", CLOCKS_PER_SEC);
	pc.printf("RAND_MAX: %d\r\n", RAND_MAX);
#endif

	u8g2Initialize();
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
	u8g2_DrawStr(&U8g2Handler, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&U8g2Handler, 0, 32, TITLE_STR2);
	u8g2_SendBuffer(&U8g2Handler);
	wait(2.0);

	debouncerInitialize();
	
	for (int i = 0; i < OSC_NUM; i++) {
		phaccu[i] = 0;
		tword_m[i] = pow(2.0, 32) * drate[i] / REF_CLOCK;  // calculate DDS tuning word;
	}
    
    // 1.0s / 1.0us = 1000000.0
    float interruptPeriodUs = 1000000.0 / REF_CLOCK; 
    ticker.attach_us(&update, interruptPeriodUs);
    
	int count = 0;
	char strBuffer[20];
	Timer t;
	t.start();
    while (1) {
#if (PIN_CHECK)
		CheckPin2.write(1);
#endif

		readAdcParameters();
		for (int i = 0; i < OSC_NUM; i++) {
			tword_m[i] = pow(2.0, 32) * drate[i] / REF_CLOCK;  // calculate DDS tuning word;
		}
		
		readButtonParameters();
		
#if (UART_TRACE)
		for (int i = 0; i < OSC_NUM; i++) {
			pc.printf("%d  %d  %3.2lf\t%1.3f\t%d\t%1.3f:\t", 
				waveShape[i],
				frequencyRange[i],
				drate[i], 
				phase[i],
				pulseWidth[i],
				amplitude[i]
			);
		}
		pc.printf("\r\n");
#endif

		float elapse_time = t.read();
		u8g2_ClearBuffer(&U8g2Handler);
		u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
		sprintf(strBuffer, "%.1f %.1fs", count/elapse_time, elapse_time);
		u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);
		sprintf(strBuffer, "%08d", count);
		u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);
		u8g2_SendBuffer(&U8g2Handler);

		count++;

#if (PIN_CHECK)
		CheckPin2.write(0);
#endif
    }
}

