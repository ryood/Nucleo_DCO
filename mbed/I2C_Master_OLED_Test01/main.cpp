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

// display mode
enum {
	DM_TITLE       = 0,
	DM_NORMAL      = 1,
	DM_FREQUENCY   = 2,
	DM_AMPLITUDE   = 3,
	DM_PULSE_WIDTH = 4,
	DM_DISPLAY_OFF = 255
};

I2C I2cArduino(PB_9, PB_8);  // SDA, SCL
DigitalOut CheckPin1(PA_10);

#if (UART_TRACE)
Serial pc(USBTX, USBRX);
#endif

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
    
    int x = 0;
    while(1) {
        CheckPin1.write(1);
#if (UART_TRACE)
		pc.printf("displayTitle()\r\n");
#endif
	
		const int len = 16;
		char strBuffer[len];

		uint8_t mode = DM_TITLE;
		if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&mode, 1, true) != 0) {
			printf("%d I2C failure: mode\r\n", x);
		}
		
		strncpy(strBuffer, TITLE_STR1, len);
		if (I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, false) != 0) {
			printf("%d I2C failure: TITLE_STR1\r\n", x);
		}
/*
		strncpy(strBuffer, TITLE_STR2, len);
		I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, true);
		strncpy(strBuffer, TITLE_STR3, len);
		I2cArduino.write(I2C_ARDUINO_ADDR, strBuffer, len, false);
*/
/*
	if (I2cArduino.write(I2C_ARDUINO_ADDR, "x is ", 5, true) != 0) {
            printf("I2C failure");
        }
        if (I2cArduino.write(I2C_ARDUINO_ADDR, (char *)&x, 1) != 0) {
            printf("I2C failure");
        }
        CheckPin1.write(0);
*/
        x++;
        wait_ms(500);
    }
}
