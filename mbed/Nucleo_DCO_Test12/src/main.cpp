/*
   Nucleo DCO Test12

   u8g2-mbed + DDS(3OSC) + InternalADC + InterruptIn + DisplayMode + FrequencyRange
   64k wavetable
   
   2018.09.13

*/
#include "mbed.h"
#include "u8g2.h"
#include <math.h>

#include "wavetable_12bit_64k.h"
#define COUNT_OF_ENTRIES  (65536)

#define PIN_CHECK   (1)
#define UART_TRACE  (1)
#define TITLE_STR1  ("DCO Test12")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define OSC_NUM              (3)
#define FREQUENCY_RANGE_MAX  (10)
#define REF_CLOCK            (100000)  // 100kHz

#define DEBOUNCE_DELAY       (10000)  // usec

// Pin Assign
AnalogOut Dac1(PA_4);
AnalogOut Dac2(PA_5);

AnalogIn Adc1(PA_3);
AnalogIn Adc2(PC_0);
AnalogIn Adc3(PC_3);
AnalogIn Adc4(PF_3);
AnalogIn Adc5(PF_5);
AnalogIn Adc6(PF_10);
AnalogIn Adc7(PA_0);
AnalogIn Adc8(PF_8);
AnalogIn Adc9(PF_7);
AnalogIn Adc10(PF_9);

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

// display mode
enum {
	DM_FPS,
	DM_WAVESHAPE_RANGE,
	DM_DETUNE,
	DM_FREQUENCY1,
	DM_FREQUENCY2,
	DM_FREQUENCY3,
	DM_MAX
};

int displayMode = DM_WAVESHAPE_RANGE;

// DDS Interruput
Ticker ddsTicker;

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

const char* waveShapeName[] = {
	"SIN",
	"TRI",
	"SUP",
	"SDN",
	"SQR",
	"NOS",
	"XXX"
};

const char* frequencyRangeName[FREQUENCY_RANGE_MAX] = {
	"A-1",
	" A0",
	" A1",
	" A2",
	" A3",
	" A4",
	" A5",
	" A6",
	" A7",
	" A8"
};

const double frequencyBase[FREQUENCY_RANGE_MAX] = {
//  A-1    A0    A1    A2     A3     A4     A5     A6      A7      A8
	13.75, 27.5, 55.0, 110.0, 220.0, 440.0, 880.0, 1760.0, 3520.0, 7040.0
};

double masterFrequency = 1000.0;
float masterAmplitude = 1.0f;

double detune[OSC_NUM]
	= { 0.0, 0.0, 0.0 };
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
	= { 2, 3, 4 };

// DDS
volatile uint32_t phaccu[OSC_NUM];
volatile uint32_t tword_m[OSC_NUM];

// Debounce
Timeout debouncer0;
Timeout debouncer1;
Timeout debouncer2;
Timeout debouncer3;
Timeout debouncer4;
Timeout debouncer5;
Timeout debouncer6;
Timeout debouncer7;

volatile bool isButtonPushed0 = false;
volatile bool isButtonPushed1 = false;
volatile bool isButtonPushed2 = false;
volatile bool isButtonPushed3 = false;
volatile bool isButtonPushed4 = false;
volatile bool isButtonPushed5 = false;
volatile bool isButtonPushed6 = false;
volatile bool isButtonPushed7 = false;

//-------------------------------------------------------------------------------------------------
// DDS Interrupt Service Routine
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
	
	// Attenuate
	sum = sum * masterAmplitude * 2.0f;
	
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

void debounce0() { if (Button0.read() == 0) isButtonPushed0 = true; }
void debounce1() { if (Button1.read() == 0) isButtonPushed1 = true; }
void debounce2() { if (Button2.read() == 0) isButtonPushed2 = true; }
void debounce3() { if (Button3.read() == 0) isButtonPushed3 = true; }
void debounce4() { if (Button4.read() == 0) isButtonPushed4 = true; }
void debounce5() { if (Button5.read() == 0) isButtonPushed5 = true; }
void debounce6() { if (Button6.read() == 0) isButtonPushed6 = true; }
void debounce7() { if (Button7.read() == 0) isButtonPushed7 = true; }

void interruptHandler0() { debouncer0.attach_us(&debounce0, DEBOUNCE_DELAY); }
void interruptHandler1() { debouncer1.attach_us(&debounce1, DEBOUNCE_DELAY); }
void interruptHandler2() { debouncer2.attach_us(&debounce2, DEBOUNCE_DELAY); }
void interruptHandler3() { debouncer3.attach_us(&debounce3, DEBOUNCE_DELAY); }
void interruptHandler4() { debouncer4.attach_us(&debounce4, DEBOUNCE_DELAY); }
void interruptHandler5() { debouncer5.attach_us(&debounce5, DEBOUNCE_DELAY); }
void interruptHandler6() { debouncer6.attach_us(&debounce6, DEBOUNCE_DELAY); }
void interruptHandler7() { debouncer7.attach_us(&debounce7, DEBOUNCE_DELAY); }

//-------------------------------------------------------------------------------------------------
// Initialize
//

void u8g2Initialize()
{
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

//-------------------------------------------------------------------------------------------------
// Input process
//

// Detuinig frequency
//
// param
//  frequency:  base frequency (0.0 ~)
//  detune:    detune ratio (-1.0 ~ 1.0)
inline double detuning(double frequency, double detune)
{
	// pseudo exp. curve
	
	if (detune > 0.0) {
		detune = detune + 1.0;
	} else {
		detune = detune * 0.5 + 1.0;
	}
	return frequency * detune;
}

void readAdcParameters()
{
	float f  = Adc1.read();

	// Frequency Range
	masterFrequency = f * frequencyBase[frequencyRange[0]] + frequencyBase[frequencyRange[0]];
	float f2 = f * frequencyBase[frequencyRange[1]] + frequencyBase[frequencyRange[1]];
	float f3 = f * frequencyBase[frequencyRange[2]] + frequencyBase[frequencyRange[2]];
	
	// OSC1
	drate[0]      = masterFrequency;
	pulseWidth[0] = Adc2.read() * COUNT_OF_ENTRIES;
	amplitude[0]  = (Adc3.read_u16() >> 12) / 15.0f;

	// OSC2
	detune[1]     = Adc4.read() * 2.0f - 1.0f;
	drate[1]      = detuning(f2, detune[1]);
	pulseWidth[1] = Adc5.read() * COUNT_OF_ENTRIES;
	amplitude[1]  = (Adc6.read_u16() >> 12) / 15.0f;

	// OSC3
	detune[2]     = Adc7.read() * 2.0f - 1.0f;
	drate[2]      = detuning(f3, detune[2]);
	pulseWidth[2] = Adc8.read() * COUNT_OF_ENTRIES;
	amplitude[2]  = (Adc9.read_u16() >> 12) / 15.0f;
	
	// Master amplitude
	masterAmplitude = Adc10.read();
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
	
	// Display Mode
	if (isButtonPushed6) {
		displayMode++;
		if (displayMode >= DM_MAX) {
			displayMode = 0;
		}
		isButtonPushed6 = false;
	}
}

//-------------------------------------------------------------------------------------------------
// Display
//

void displayFps(float elapse, int count)
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	sprintf(strBuffer, "%.1f %.1fs", count/elapse, elapse);
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);

	sprintf(strBuffer, "%08d", count);
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);

	u8g2_SendBuffer(&U8g2Handler);
}

void displayWaveShapeRange()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	sprintf(strBuffer, "%s %s %s", 
		waveShapeName[waveShape[0]],
		waveShapeName[waveShape[1]],
		waveShapeName[waveShape[2]]
	);
	u8g2_DrawStr(&U8g2Handler, 4, 16, strBuffer);

	sprintf(strBuffer, "%s %s %s", 
		frequencyRangeName[frequencyRange[0]],
		frequencyRangeName[frequencyRange[1]],
		frequencyRangeName[frequencyRange[2]]
	);
	u8g2_DrawStr(&U8g2Handler, 4, 32, strBuffer);

	u8g2_SendBuffer(&U8g2Handler);
}

void displayDetune()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	sprintf(strBuffer, "%9.3lf Hz", masterFrequency); 
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);

	sprintf(strBuffer, "%6.3lf %6.3lf", detune[1], detune[2]);
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);

	u8g2_SendBuffer(&U8g2Handler);
}

void displayFrequency(int n)
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	sprintf(strBuffer, "#%d %6.3lf", n+1, detune[n]);
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);


	sprintf(strBuffer, "%9.3lf Hz", drate[n]); 
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);

	u8g2_SendBuffer(&U8g2Handler);
}

//-------------------------------------------------------------------------------------------------
// Main function
//
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
    ddsTicker.attach_us(&update, interruptPeriodUs);
    
	int count = 0;
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
				detune[i],
				pulseWidth[i],
				amplitude[i]
			);
		}
		pc.printf("%1.3f\t%d\r\n", masterAmplitude, displayMode);
#endif
		
		float elapseTime = t.read();
		if (elapseTime > 1.0f) {
			t.reset();
			count = 0;
		}
		switch (displayMode) {
		case DM_FPS:
			displayFps(elapseTime, count);
			break;
		case DM_WAVESHAPE_RANGE:
			displayWaveShapeRange();
			break;
		case DM_DETUNE:
			displayDetune();
			break;
		case DM_FREQUENCY1:
			displayFrequency(0);
			break;
		case DM_FREQUENCY2:
			displayFrequency(1);
			break;
		case DM_FREQUENCY3:
			displayFrequency(2);
			break;
		}

		count++;

#if (PIN_CHECK)
		CheckPin2.write(0);
#endif
    }
}

