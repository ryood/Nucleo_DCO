/*
 * Nucleo DCO I2C Slave / OLED Module Test.
 *
 * 2018.11.05
 *
 */
 
#include "mbed.h"

#define UART_TRACE  (1)

#define I2C_CLOCK (100000)
#define I2C_ARDUINO_ADDR   (0x08 << 1)  // 8bit address
#define TITLE_STR1  ("I2C OLED Test")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define OSC_NUM  (3)

// display mode
enum {
	DM_TITLE       = 0,
	DM_NORMAL      = 1,
	DM_FREQUENCY   = 2,
	DM_AMPLITUDE   = 3,
	DM_PULSE_WIDTH = 4,
	DM_TITLE_STR1  = 128,
	DM_TITLE_STR2  = 129,
	DM_TITLE_STR3  = 130,
	DM_DISPLAY_OFF = 255
};

I2C I2cArduino(PB_9, PB_8);  // SDA, SCL
DigitalOut CheckPin1(PA_10);

#if (UART_TRACE)
Serial pc(USBTX, USBRX);
#endif

// I2C通信用構造体
#pragma pack(1)
struct normalData {
	uint8_t  waveShape[OSC_NUM];
	uint8_t  frequencyRange[OSC_NUM];
	uint16_t fps;
	uint8_t  batteryVoltage;
	bool     adcAvailable;
};

// parameter
volatile int waveShape[OSC_NUM];
volatile int frequencyRange[OSC_NUM];
volatile int fps;
volatile int batteryVoltage;
volatile bool adcAvailable;

int displayMode;
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
	
	// initialize parameter
	for (int i = 0; i < OSC_NUM; i++) {
		waveShape[i] = i + 1;
	}
	for (int i = 0; i < OSC_NUM; i++) {
		frequencyRange[i] = i + 3;
	}
	fps = 600;            // 60.0fps
	batteryVoltage = 90;  // 9.0V
	adcAvailable = true;
    
	displayMode = DM_NORMAL;
	
    while(1) {
        CheckPin1.write(1);
		switch (displayMode) {
		case DM_TITLE:
			displayTitle();
			break;
		case DM_NORMAL:
			displayNormal();
			break;
		}
        CheckPin1.write(0);
		
        x++;
        wait_ms(500);
    }
}
