/*
 * Nucleo DCO I2C Slave / OLED Module Test.
 *
 * 2018.11.05
 *
 */
 
#include "mbed.h"
#include "DataComFormat.h"

#define UART_TRACE  (1)

#define I2C_CLOCK (100000)
#define I2C_ARDUINO_ADDR   (0x08 << 1)  // 8bit address
#define TITLE_STR1  ("I2C OLED Test")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

int displayMode = DM_NORMAL;

I2C I2cArduino(PB_9, PB_8);  // SDA, SCL
DigitalOut CheckPin1(PA_10);

#if (UART_TRACE)
Serial pc(USBTX, USBRX);
#endif

// parameter
volatile int waveShape[OSC_NUM];
volatile int frequencyRange[OSC_NUM];
volatile int fps;
volatile int batteryVoltage;
volatile bool adcAvailable;

volatile double drate[OSC_NUM];
volatile double detune[OSC_NUM];

volatile float amplitude[OSC_NUM];
volatile float masterAmplitude;
volatile bool isClip;

volatile int pulseWidth[OSC_NUM];

int x = 0;

void displayTitle()
{
#if (UART_TRACE)
	pc.printf("displayTitle()\r\n");
#endif

	const int len = 32;
	char strBuffer[len];

	// Title文字列を送信
	strncpy(strBuffer, TITLE_STR1, len);
	printf("%s %d %d\r\n", strBuffer, len, strlen(strBuffer));
	uint8_t mode = DM_TITLE_STR1;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, false) != 0) {
		printf("%d I2C failure: TitleStr1\r\n", x);
	}
	
	strncpy(strBuffer, TITLE_STR2, len);
	printf("%s %d %d\r\n", strBuffer, len, strlen(strBuffer));
	mode = DM_TITLE_STR2;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, false) != 0) {
		printf("%d I2C failure: TitleStr2\r\n", x);
	}

	strncpy(strBuffer, TITLE_STR3, len);
	printf("%s %d %d\r\n", strBuffer, len, strlen(strBuffer));
	mode = DM_TITLE_STR3;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, false) != 0) {
		printf("%d I2C failure: TitleStr3\r\n", x);
	}
	
	wait_ms(1);
	
	// Title表示指示を送信
	mode = DM_TITLE;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, false) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
}

void displayNormal()
{
#if (UART_TRACE)
	pc.printf("displayNormal()\r\n");
#endif

	struct normalData d;

	d.waveShape[0] = waveShape[0];
	d.waveShape[1] = waveShape[1];
	d.waveShape[2] = waveShape[2];
	d.frequencyRange[0] = frequencyRange[0];
	d.frequencyRange[1] = frequencyRange[1];
	d.frequencyRange[2] = frequencyRange[2];
	d.fps = fps;
	d.batteryVoltage = batteryVoltage;
	d.adcAvailable = adcAvailable;
	
#if (UART_TRACE)
	pc.printf("WaveShape1: %d\r\n", d.waveShape[0]);
	pc.printf("WaveShape2: %d\r\n", d.waveShape[1]);
	pc.printf("WaveShape3: %d\r\n", d.waveShape[2]);
	pc.printf("FreqRange1: %d\r\n", d.frequencyRange[0]);
	pc.printf("FreqRange2: %d\r\n", d.frequencyRange[1]);
	pc.printf("FreqRange3: %d\r\n", d.frequencyRange[2]);
	pc.printf("FPS: %d\r\n", d.fps);
	pc.printf("BattVoltage: %d\r\n", d.batteryVoltage);
	pc.printf("ADCAvailable: %d\r\n", d.adcAvailable);
#endif	
	
	uint8_t mode = DM_NORMAL;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&d, sizeof(d), false) != 0) {
		printf("%d I2C failure: normalData\r\n", x);
	}
}

void displayFrequency()
{
#if (UART_TRACE)
	pc.printf("displayFrequency()\r\n");
#endif

	struct frequencyData d;
	
	d.rate[0] = drate[0] * 10;
	d.rate[1] = drate[1] * 10;
	d.rate[2] = drate[2] * 10;
	d.detune[0] = detune[0] * 1000;
	d.detune[1] = detune[1] * 1000;
	d.detune[2] = detune[2] * 1000;

#if (UART_TRACE)
	pc.printf("Rate1: %d\r\n", d.rate[0]);
	pc.printf("Rate2: %d\r\n", d.rate[1]);
	pc.printf("Rate3: %d\r\n", d.rate[2]);
	pc.printf("Detune1: %d\r\n", d.detune[0]);
	pc.printf("Detune2: %d\r\n", d.detune[1]);
	pc.printf("Detune3: %d\r\n", d.detune[2]);
#endif

	uint8_t mode = DM_FREQUENCY;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&d, sizeof(d), false) != 0) {
		printf("%d I2C failure: frequencyData\r\n", x);
	}
}

void displayAmplitude()
{
#if (UART_TRACE)
	pc.printf("displayAmplitude()\r\n");
#endif

	struct amplitudeData d;
	
	d.amplitude[0] = amplitude[0] * 1000;
	d.amplitude[1] = amplitude[1] * 1000;
	d.amplitude[2] = amplitude[2] * 1000;
	d.masterAmplitude = masterAmplitude * 1000;
	d.clip = isClip;

#if (UART_TRACE)
	pc.printf("Amplitude1: %d\r\n", d.amplitude[0]);
	pc.printf("Amplitude2: %d\r\n", d.amplitude[1]);
	pc.printf("Amplitude3: %d\r\n", d.amplitude[2]);
	pc.printf("MasterAmplitude: %d\r\n", d.masterAmplitude);
	pc.printf("Clip: %d\r\n", d.clip);
#endif

	uint8_t mode = DM_AMPLITUDE;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&d, sizeof(d), false) != 0) {
		printf("%d I2C failure: amplitudeData\r\n", x);
	}
}

void displayPulseWidth()
{
#if (UART_TRACE)
	pc.printf("displayPulseWidth()\r\n");
#endif

	struct pulseWidthData d;
	
	d.pulseWidth[0] = ((float)pulseWidth[0] / UINT16_MAX) * 1000;
	d.pulseWidth[1] = ((float)pulseWidth[1] / UINT16_MAX) * 1000;
	d.pulseWidth[2] = ((float)pulseWidth[2] / UINT16_MAX) * 1000;

#if (UART_TRACE)
	pc.printf("PulseWidth1: %d\r\n", d.pulseWidth[0]);
	pc.printf("PulseWidth2: %d\r\n", d.pulseWidth[1]);
	pc.printf("PulseWidth2: %d\r\n", d.pulseWidth[2]);
#endif

	uint8_t mode = DM_PULSE_WIDTH;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&d, sizeof(d), false) != 0) {
		printf("%d I2C failure: pulseWidthData\r\n", x);
	}
}

void displayOff()
{
#if (UART_TRACE)
	pc.printf("displayOff()\r\n");
#endif

	uint8_t mode = DM_DISPLAY_OFF;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
		printf("%d I2C failure: mode %d\r\n", x, mode);
	}
	uint8_t message = false;
	if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&message, 1, false) != 0) {
		printf("%d I2C failure: displayOff message\r\n", x);
	}
}

int main()
{
#if (UART_TRACE)
	pc.baud(115200);
	pc.printf("\r\n%s\r\n", TITLE_STR1);
	pc.printf("%s\r\n", TITLE_STR2);
	pc.printf("%s\r\n", TITLE_STR3);
	wait(1.0);
#endif

    I2cArduino.frequency(I2C_CLOCK);
	
	// initialize parameter (dummy)
	waveShape[0] = 1;
	waveShape[1] = 2;
	waveShape[2] = 3;
	frequencyRange[0] = 4;
	frequencyRange[1] = 5;
	frequencyRange[2] = 6;
	fps = 600;            // 60.0fps
	batteryVoltage = 90;  // 9.0V
	adcAvailable = true;
	
	drate[0] = 440.1;
	drate[1] = 880.2;
	drate[2] = 1320.3;
	detune[0] = 0.0;
	detune[1] = -0.543;
	detune[2] = 0.543;
	
	amplitude[0] = 0.123f;
	amplitude[1] = 0.456f;
	amplitude[2] = 0.567f;
	masterAmplitude = 1.000f;
	isClip = true;
	
	pulseWidth[0] = 0;
	pulseWidth[1] = 32768;
	pulseWidth[2] = 65536;
	
    while(1) {
        CheckPin1.write(1);
		switch (displayMode) {
		case DM_TITLE:
			displayTitle();
			break;
		case DM_NORMAL:
			displayNormal();
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
		case DM_DISPLAY_OFF:
			displayOff();
			break;
		}
        CheckPin1.write(0);
		
        x++;
        wait_ms(500);
    }
}
