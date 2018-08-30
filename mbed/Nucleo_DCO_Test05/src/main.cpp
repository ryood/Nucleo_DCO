/*
   Nucleo DCO Test05

   3OSC Mix Test
   
   2018.08.31

*/
#include "mbed.h"
#include "u8g2.h"

#include "wavetable_12bit_32k.h"
#define COUNT_OF_ENTRIES  (32768)

#define PIN_CHECK   (1)
#define UART_TRACE  (1)
#define TITLE_STR1  ("DCO Test05")
#define TITLE_STR2  ("2018.08.31")

// Pin Assign
AnalogOut Dac1(PA_4);
AnalogOut Dac2(PA_5);

#if (PIN_CHECK)
DigitalOut CheckPin1(D4);
DigitalOut CheckPin2(D5);
#endif

// u8g2
uint8_t u8x8_gpio_and_delay_mbed(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
u8g2_t U8g2Handler;

// Parameter
double drate = 1000.0;         // initial output rate (Hz)
const double refclk = 100000;  // 100kHz

// Interruput
Ticker ticker;

// DDS
volatile uint32_t phaccu;
volatile uint32_t tword_m;

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

    phaccu = phaccu + tword_m;
    uint16_t idx = phaccu >> 17;  // use upper 15 bits
    
    internalDacWrite(1, sin_12bit_32k[idx]);
    //internalDacWrite(2, sin_12bit_32k[idx]);

#if (PIN_CHECK)
    CheckPin1.write(0);
#endif
}

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

	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
	u8g2_DrawStr(&U8g2Handler, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&U8g2Handler, 0, 32, TITLE_STR2);
	u8g2_SendBuffer(&U8g2Handler);
}

int main()
{
#if (UART_TRACE)
	printf("\r\n%s\r\n", TITLE_STR1);
	printf("%s\r\n", TITLE_STR2);
	printf("System Clock: %d Hz\r\n", SystemCoreClock);
	printf("CLOCKS_PER_SEC: %ld\n", CLOCKS_PER_SEC); 
#endif

    tword_m = pow(2.0, 32) * drate / refclk;  // calculate DDS tuning word;
    
    // 1.0s / 1.0us = 1000000.0
    float interruptPeriodUs = 1000000.0 / refclk; 
    ticker.attach_us(&update, interruptPeriodUs);

	u8g2Initialize();
	wait(2.0);
    
	int count = 0;
	char strBuffer[20];
	Timer t;
	t.start();
    while (1) {
#if (PIN_CHECK)
		CheckPin2.write(1);
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

