/*
   Nucleo DCO Test21

   u8g2-mbed + DDS(3OSC) + InternalADC + InterruptIn + DisplayMode + FrequencyRange
   64k wavetable
   SSD1306 128x64 I2C
   Interpolate ADC
   ADC: n bit precision
   Rotary Encoders
   Leds
   UART Trace FPS
   Power voltage check
   Thread
   
   2018.10.01

*/
#include "mbed.h"
#include "u8g2.h"
#include "InterpolateFloat.h"
#include "RotaryEncoder.h"
#include <math.h>

#include "wavetable_12bit_64k.h"
#define COUNT_OF_ENTRIES  (65536)

#define PIN_CHECK   (1)
#define UART_TRACE  (1)
#define TITLE_STR1  ("DCO Test21")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define OSC_NUM              (3)
#define FREQUENCY_RANGE_MAX  (10)
#define REF_CLOCK            (62500)  // Hz

#define DEBOUNCE_DELAY       (20000)  // usec
#define ROT_ENC_INTERVAL     (2000)   // usec

#define INTERPOLATE_DIVISION (64)
#define PRUNING_FACTOR       (1.05f)

#define MASTER_FREQUENCY_OCT (3.0f)

#define INTERNAL_VOLTAGE     (1.22f)
#define VOLTAGE_DIV_R1       (12.0f)
#define VOLTAGE_DIV_R2       (3.3f)
#define VOLTAGE_DIVIDE       ((VOLTAGE_DIV_R1 + VOLTAGE_DIV_R2) / VOLTAGE_DIV_R2)

enum {
	WS_SIN,
	WS_TRI,
	WS_SAWUP,
	WS_SAWDOWN,
	WS_SQR,
	WS_NOISE,
	WS_MAX
};

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
AnalogIn Adc11(PB_1);
AnalogIn AdcVref(ADC_VREF);

// ADC Interpolate
InterpolateFloat iAdc1(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc2(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc3(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc4(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc5(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc6(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc7(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc8(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc9(0, INTERPOLATE_DIVISION);
InterpolateFloat iAdc10(0, INTERPOLATE_DIVISION);

// Interrupt: cannot use same pin number
InterruptIn Button1(PE_8, PullUp);
InterruptIn Button2(PE_7, PullUp);
InterruptIn Button3(PE_10, PullUp);
InterruptIn Button4(PE_12, PullUp);

// Rotary Encoder
RotaryEncoder RotEnc1(PC_8,  PC_9,  0, WS_MAX - 1, WS_SIN);
RotaryEncoder RotEnc2(PC_10, PC_11, 0, FREQUENCY_RANGE_MAX - 1, 4);
RotaryEncoder RotEnc3(PC_12, PD_2,  0, WS_MAX - 1, WS_SIN);
RotaryEncoder RotEnc4(PG_2,  PG_3,  0, FREQUENCY_RANGE_MAX - 1, 4);
RotaryEncoder RotEnc5(PD_7,  PD_6,  0, WS_MAX - 1, WS_SIN);
RotaryEncoder RotEnc6(PD_5,  PD_4,  0, FREQUENCY_RANGE_MAX - 1, 4);

// LED
DigitalOut Led1(PC_6);
DigitalOut Led2(PB_13); // Recovering wiring fail
DigitalOut Led3(PB_15); // Recovering wiring fail
DigitalOut Led4(PB_12);

#if (PIN_CHECK)
DigitalOut CheckPin1(D4);
DigitalOut CheckPin2(D5);
#endif

// u8g2
uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t U8g2Handler;

// display mode
enum {
	DM_TITLE,
	DM_NORMAL,
	DM_FREQUENCY,
	DM_AMPLITUDE,
	DM_PULSE_WIDTH,
	DM_MAX
};

volatile int displayMode = DM_NORMAL;
volatile bool isDisplayOff = false;
volatile bool toDisplayOffMessage = false;

// DDS Interruput
Ticker ddsTicker;

// Thread
Thread threadDisplay;

// UART
#if (UART_TRACE)
Serial pc(USBTX, USBRX);
#endif

// Const strings

const char* waveShapeName[] = {
	"SIN",
	"TRI",
	"SWU",
	"SWD",
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

// Parameter

volatile double masterFrequency = 1000.0;
volatile float masterAmplitude = 1.0f;

volatile double detune[OSC_NUM]
	= { 0.0, 0.0, 0.0 };
volatile double drate[OSC_NUM]          // output rate (Hz)
	= { 1000.0, 1000.0, 1000.0 };
volatile float phase[OSC_NUM]           // output phase (0.0 ~ 1.0)
	= { 0.0f, 0.0f, 0.0f };
volatile int pulseWidth[OSC_NUM]        // output pulseWidth (0 ~ COUNT_OF_ENTRIES)
	= { COUNT_OF_ENTRIES/2, COUNT_OF_ENTRIES/2, COUNT_OF_ENTRIES/2 };
volatile float amplitude[OSC_NUM]       // output amplitude (0.0 ~ 1.0)
	= { 0.3f, 0.3f, 0.3f };
volatile int waveShape[OSC_NUM]
	= { WS_SIN, WS_SIN, WS_SIN };
volatile int frequencyRange[OSC_NUM]
	= { 4, 4, 4 };

// DDS
volatile uint32_t phaccu[OSC_NUM];
volatile uint32_t tword_m[OSC_NUM];

// Debounce
Timeout debouncer1;
Timeout debouncer2;
Timeout debouncer3;
Timeout debouncer4;

volatile bool isButtonPushed1 = false;
volatile bool isButtonPushed2 = false;
volatile bool isButtonPushed3 = false;
volatile bool isButtonPushed4 = false;

// Power voltage
volatile float powerVoltage;

// Suppress ADC
volatile bool isSuppressAdc = false;

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

// Detuinig frequency
//
// param
//  frequency:  base frequency (0.0 ~)
//  detune:    detune ratio (-1.0 ~ 1.0)
inline
double detuning(double frequency, double detune)
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
	float f  = MASTER_FREQUENCY_OCT * iAdc1.get();

	// Frequency Range
	masterFrequency = f * frequencyBase[frequencyRange[0]] + frequencyBase[frequencyRange[0]];
	float f2 = f * frequencyBase[frequencyRange[1]] + frequencyBase[frequencyRange[1]];
	float f3 = f * frequencyBase[frequencyRange[2]] + frequencyBase[frequencyRange[2]];
	
	// OSC1
	drate[0]      = masterFrequency;
	pulseWidth[0] = iAdc2.get() * COUNT_OF_ENTRIES;
	amplitude[0]  = iAdc3.get();

	// OSC2
	detune[1]     = iAdc4.get() * 2.0f - 1.0f;
	drate[1]      = detuning(f2, detune[1]);
	pulseWidth[1] = iAdc5.get() * COUNT_OF_ENTRIES;
	amplitude[1]  = iAdc6.get();

	// OSC3
	detune[2]     = iAdc7.get() * 2.0f - 1.0f;
	drate[2]      = detuning(f3, detune[2]);
	pulseWidth[2] = iAdc8.get() * COUNT_OF_ENTRIES;
	amplitude[2]  = iAdc9.get();
	
	// Master amplitude
	masterAmplitude = iAdc10.get();
}

void update()
{
#if (PIN_CHECK)
    CheckPin1.write(1);
#endif

	readAdcParameters();
	for (int i = 0; i < OSC_NUM; i++) {
		tword_m[i] = pow(2.0, 32) * drate[i] / REF_CLOCK;  // calculate DDS tuning word;
	}

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
		Led4 = 1;
	}
	else if (sum > 4095) {
		sum = 4095;
		Led4 = 1;
	}
	else {
		Led4 = 0;
	}

	internalDacWrite(1, (uint16_t)sum);

#if (PIN_CHECK)
    CheckPin1.write(0);
#endif
}

//-------------------------------------------------------------------------------------------------
// Debounce
//

void debounce1() { if (Button1.read() == 0) isButtonPushed1 = true; }
void debounce2() { if (Button2.read() == 0) isButtonPushed2 = true; }
void debounce3() { if (Button3.read() == 0) isButtonPushed3 = true; }
void debounce4() { if (Button4.read() == 0) isButtonPushed4 = true; }

void interruptHandler1() { debouncer1.attach_us(&debounce1, DEBOUNCE_DELAY); }
void interruptHandler2() { debouncer2.attach_us(&debounce2, DEBOUNCE_DELAY); }
void interruptHandler3() { debouncer3.attach_us(&debounce3, DEBOUNCE_DELAY); }
void interruptHandler4() { debouncer4.attach_us(&debounce4, DEBOUNCE_DELAY); }

//-------------------------------------------------------------------------------------------------
// Initialize
//

void u8g2Initialize()
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&U8g2Handler, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_mbed);
	u8g2_InitDisplay(&U8g2Handler);
	u8g2_SetPowerSave(&U8g2Handler, 0);
}

void debouncerInitialize()
{
	Button1.fall(&interruptHandler1);
	Button2.fall(&interruptHandler2);
	Button3.fall(&interruptHandler3);
	Button4.fall(&interruptHandler4);
}

void rotEncInitialize()
{
	RotEnc1.setInterval(ROT_ENC_INTERVAL);
	RotEnc2.setInterval(ROT_ENC_INTERVAL);
	RotEnc3.setInterval(ROT_ENC_INTERVAL);
	RotEnc4.setInterval(ROT_ENC_INTERVAL);
	RotEnc5.setInterval(ROT_ENC_INTERVAL);
	RotEnc6.setInterval(ROT_ENC_INTERVAL);
}

void ledsCheck()
{
	const float ledWait = 0.1f;
	
	for (int i = 0; i < 4; i++) {
		Led1.write(1);
		wait(ledWait);
		Led1.write(0);
		
		Led2.write(1);
		wait(ledWait);
		Led2.write(0);
		
		Led3.write(1);
		wait(ledWait);
		Led3.write(0);

		Led4.write(1);
		wait(ledWait);
		Led4.write(0);
	}
}

//-------------------------------------------------------------------------------------------------
// Input process
//

inline
float adcRead6bit(AnalogIn& adc)
{
	uint16_t v = adc.read_u16();
	v = v >> 10;
	return (float)v / 63.0f;
}

inline
float adcRead7bit(AnalogIn& adc)
{
	uint16_t v = adc.read_u16();
	v = v >> 9;
	return (float)v / 127.0f;
}

inline
float adcRead8bit(AnalogIn& adc)
{
	uint16_t v = adc.read_u16();
	v = v >> 8;
	return (float)v / 255.0f;
}

inline
float adcRead10bit(AnalogIn& adc)
{
	uint16_t v = adc.read_u16();
	v = v >> 6;
	return (float)v / 1023.0f;
}

inline
float adcRead12bit(AnalogIn& adc)
{
	uint16_t v = adc.read_u16();
	v = v >> 4;
	return (float)v / 4095.0f;
}

inline 
float pruning(float v)
{
	float ret = ((v - 0.5f) * PRUNING_FACTOR) + 0.5f;
	if (ret < 0.0f) {
		ret = 0.0f;
	} else if (ret > 1.0f) {
		ret = 1.0f;
	}
	return ret;
}

void readAdc()
{
	iAdc1.setNext(pruning(adcRead12bit(Adc1)));
	iAdc2.setNext(pruning(adcRead12bit(Adc2)));
	iAdc3.setNext(pruning(adcRead12bit(Adc3)));
	iAdc4.setNext(pruning(adcRead12bit(Adc4)));
	iAdc5.setNext(pruning(adcRead12bit(Adc5)));
	iAdc6.setNext(pruning(adcRead12bit(Adc6)));
	iAdc7.setNext(pruning(adcRead12bit(Adc7)));
	iAdc8.setNext(pruning(adcRead12bit(Adc8)));
	iAdc9.setNext(pruning(adcRead12bit(Adc9)));
	iAdc10.setNext(pruning(adcRead12bit(Adc10)));
	
	float vref = AdcVref.read();
	float vrefFactor = INTERNAL_VOLTAGE / vref;
	float v = Adc11.read();
	powerVoltage = v * vrefFactor * VOLTAGE_DIVIDE;
}

void readButtonParameters()
{
	// Display Mode
	if (isButtonPushed1) {
		displayMode++;
		if (displayMode >= DM_MAX) {
			displayMode = 0;
		}
		isButtonPushed1 = false;
	}
	
	// Display Off
	if (isButtonPushed2) {
		isDisplayOff = !isDisplayOff;
		toDisplayOffMessage = true;
		isButtonPushed2 = false;
	}
	
	// ADC Off
	if (isButtonPushed3) {
		isSuppressAdc = !isSuppressAdc;
		isButtonPushed3 = false;
	}
}

void readRotEncParameters()
{
	waveShape[0]      = RotEnc1.getVal();
	frequencyRange[0] = RotEnc2.getVal();
	waveShape[1]      = RotEnc3.getVal();
	frequencyRange[1] = RotEnc4.getVal();
	waveShape[2]      = RotEnc5.getVal();
	frequencyRange[2] = RotEnc6.getVal();
}

//-------------------------------------------------------------------------------------------------
// Display
//

void displayTitle()
{
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
	u8g2_DrawStr(&U8g2Handler, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&U8g2Handler, 0, 32, TITLE_STR2);
	u8g2_DrawStr(&U8g2Handler, 0, 48, TITLE_STR3);
	u8g2_SendBuffer(&U8g2Handler);
}

void displayNormal(float fps)
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

	sprintf(strBuffer, "FPS:%.1f", fps);
	u8g2_DrawStr(&U8g2Handler, 4, 48, strBuffer);
	
	sprintf(strBuffer, "BAT:%4.1fV %s", powerVoltage, isSuppressAdc ? "xx" : "AD");
	u8g2_DrawStr(&U8g2Handler, 4, 64, strBuffer);
	
	u8g2_SendBuffer(&U8g2Handler);
}

void displayFrequency()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	// Frequency
	sprintf(strBuffer, "F1:%7.1lf Hz", drate[0]); 
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);
	sprintf(strBuffer, "F2:%7.1lf Hz", drate[1]); 
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);
	sprintf(strBuffer, "F3:%7.1lf Hz", drate[2]); 
	u8g2_DrawStr(&U8g2Handler, 0, 48, strBuffer);
	
	// Detune
	sprintf(strBuffer, "%6.3lf %6.3lf", detune[1], detune[2]);
	u8g2_DrawStr(&U8g2Handler, 0, 64, strBuffer);

	u8g2_SendBuffer(&U8g2Handler);
}

void displayAmplitude()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	// Amplitude
	sprintf(strBuffer, "AMP1: %1.3lf ", amplitude[0]); 
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);
	sprintf(strBuffer, "AMP2: %1.3lf ", amplitude[1]); 
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);
	sprintf(strBuffer, "AMP3: %1.3lf ", amplitude[2]); 
	u8g2_DrawStr(&U8g2Handler, 0, 48, strBuffer);
	sprintf(strBuffer, "MAMP: %1.3lf ", masterAmplitude); 
	u8g2_DrawStr(&U8g2Handler, 0, 64, strBuffer);
	
	u8g2_SendBuffer(&U8g2Handler);
}

void displayPulseWidth()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	// Pulse Width
	sprintf(strBuffer, "PW1: %1.3lf ", (float)pulseWidth[0] / UINT16_MAX); 
	u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);
	sprintf(strBuffer, "PW2: %1.3lf ", (float)pulseWidth[1] / UINT16_MAX); 
	u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);
	sprintf(strBuffer, "PW3: %1.3lf ", (float)pulseWidth[2] / UINT16_MAX); 
	u8g2_DrawStr(&U8g2Handler, 0, 48, strBuffer);
	
	u8g2_SendBuffer(&U8g2Handler);
}

void displayOffMessage()
{
	char strBuffer[20];
	
	u8g2_ClearBuffer(&U8g2Handler);
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);

	sprintf(strBuffer, "DISPLAY OFF"); 
	u8g2_DrawStr(&U8g2Handler, 4, 24, strBuffer);
	
	u8g2_SendBuffer(&U8g2Handler);
}

void ledsWrite()
{
	// Pulse width  available
	if (waveShape[0] == WS_SQR) { Led1 = 1; } else { Led1 = 0; }
	if (waveShape[1] == WS_SQR) { Led2 = 1; } else { Led2 = 0; }
	if (waveShape[2] == WS_SQR) { Led3 = 1; } else { Led3 = 0; }
}

//-------------------------------------------------------------------------------------------------
// Thread
//

void displayHandler()
{
	int count = 0;
	bool toResetCount = false;
	Timer t;
	
	t.start();
	while (1) {
		float elapseTime = t.read();
		if (elapseTime > 10.0f) {
			t.reset();
			count = 0;
		}
		
		if (isDisplayOff) {
			if (toDisplayOffMessage) {
				displayOffMessage();
				toDisplayOffMessage = false;
			}
			toResetCount = true;
		}
		else {
			if (toResetCount) {
				toResetCount = false;
				t.reset();
				count = 0;
			}
			switch (displayMode) {
			case DM_TITLE:
				displayTitle();
				break;
			case DM_NORMAL:
				displayNormal(count/elapseTime);
				break;
			case DM_FREQUENCY:
				displayFrequency();
				break;
			case DM_AMPLITUDE:
				displayAmplitude();
				break;
			case DM_PULSE_WIDTH:
				displayPulseWidth();
				break;
			}
		}
		count++;
	}
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
	pc.printf("INTERNAL_VOLTAGE: %f\r\n", INTERNAL_VOLTAGE);
#endif

	u8g2Initialize();
	displayTitle();

	ledsCheck();

	debouncerInitialize();
	rotEncInitialize();
	
	for (int i = 0; i < OSC_NUM; i++) {
		phaccu[i] = 0;
		tword_m[i] = pow(2.0, 32) * drate[i] / REF_CLOCK;  // calculate DDS tuning word;
	}
    
    // 1.0s / 1.0us = 1000000.0
    float interruptPeriodUs = 1000000.0 / REF_CLOCK; 
    ddsTicker.attach_us(&update, interruptPeriodUs);
	
	// Threads start
	threadDisplay.set_priority(osPriorityLow);
	threadDisplay.start(displayHandler);

#if (UART_TRACE)
	int count = 0;
	Timer t;
	t.start();
#endif 

    while (1) {
#if (PIN_CHECK)
		CheckPin2.write(1);
#endif
		if (!isSuppressAdc) {
			readAdc();
		}
		readButtonParameters();
		readRotEncParameters();

		ledsWrite();

#if (UART_TRACE)
		float elapseTime = t.read();
		if (elapseTime > 10.0f) {
			t.reset();
			count = 0;
		}

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
		pc.printf("MasterAmplitude:%1.3f\t", masterAmplitude);
		pc.printf("DisplayMode:%d\t", displayMode);
		pc.printf("%4.1f fps\t", count/elapseTime);
		pc.printf("%5.2f V\t", powerVoltage);
		pc.printf("SuppressADC:%d\t", isSuppressAdc);
		pc.printf("\r\n");
		
		count++;
#endif

#if (PIN_CHECK)
		CheckPin2.write(0);
#endif
    }
}

