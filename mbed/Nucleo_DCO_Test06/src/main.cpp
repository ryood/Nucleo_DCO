/*
   Nucleo DCO Test06

   MCP3008 Test
   
   2018.09.01

*/
#include "mbed.h"
#include "u8g2.h"
#include "mcp3008.h"
#include <math.h>

#include "wavetable_12bit_32k.h"
#define COUNT_OF_ENTRIES  (32768)

#define PIN_CHECK   (1)
#define UART_TRACE  (1)
#define TITLE_STR1  ("DCO Test06")
#define TITLE_STR2  ("2018.09.01")

#define OSC_NUM    (3)
#define REF_CLOCK  (100000.0)  // Sampling Rate
#define SPI1_CLOCK (2000000)

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

// MCP3008
//SPI (PinName mosi, PinName miso, PinName sclk, PinName ssel=NC)
SPI SpiM1(D11, D12, D13);
MCP3008 Mcp3008_0(&SpiM1, D10);
MCP3008 Mcp3008_1(&SpiM1, D9);

// Interruput
Ticker ticker;

// DDS
double drate[OSC_NUM]          // output rate (Hz)
	= { 100.0, 200.0, 400.0 };
float amplitude[OSC_NUM]       // output amplitude (0.0 ~ 1.0)
	= { 0.5f, 0.3f, 0.2f };
float phase[OSC_NUM]           // output phase (0.0 ~ 1.0)
	= { 0.0f, 0.0f, 0.0f };
	
volatile uint32_t phaccu[OSC_NUM];
volatile uint32_t tword_m[OSC_NUM];

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
		uint16_t idx = (phaccu[i] + phi) >> 17;  // use upper 15 bits

		// Mix
		sum += sin_12bit_32k[idx] * amplitude[i];
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

int main()
{
#if (UART_TRACE)
	printf("\r\n%s\r\n", TITLE_STR1);
	printf("%s\r\n", TITLE_STR2);
	printf("UINT32_MAX: %lu\r\n", UINT32_MAX);
	printf("System Clock: %lu Hz\r\n", SystemCoreClock);
	printf("CLOCKS_PER_SEC: %d\r\n", CLOCKS_PER_SEC);

	wait(1.0);
#endif

	SpiM1.frequency(SPI1_CLOCK);
	
	for (int i = 0; i < OSC_NUM; i++) {
		phaccu[i] = 0;
		tword_m[i] = pow(2.0, 32) * drate[i] / REF_CLOCK;  // calculate DDS tuning word;
	}
    
    // 1.0s / 1.0us = 1000000.0
    float interruptPeriodUs = 1000000.0 / REF_CLOCK; 
    ticker.attach_us(&update, interruptPeriodUs);

	u8g2Initialize();
	u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
	u8g2_DrawStr(&U8g2Handler, 0, 16, TITLE_STR1);
	u8g2_DrawStr(&U8g2Handler, 0, 32, TITLE_STR2);
	u8g2_SendBuffer(&U8g2Handler);
	wait(2.0);
    
	int count = 0;
	char strBuffer[20];
	Timer t;
	t.start();
    while (1) {
#if (PIN_CHECK)
		CheckPin2.write(1);
#endif

		// MCP3008
		uint16_t v0[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
		uint16_t v1[8] = { 8, 9, 10, 11, 12, 13, 14, 15 };
        /*
		for (int i = 0; i < 8; i++) {
            v0[i] = Mcp3008_0.read_input_u16(i);
            v1[i] = Mcp3008_1.read_input_u16(i);
        }
		*/
		for (int i = 0; i < 8; i++) {
            printf("%4d\t", v0[i]);
        }
        printf(": ");
        for (int i = 0; i < 8; i++) {
            printf("%4d\t", v1[i]);
        }
		printf("\r\n");
		
		
		float elapse_time = t.read();
		u8g2_ClearBuffer(&U8g2Handler);
		u8g2_SetFont(&U8g2Handler, u8g2_font_10x20_mf);
		sprintf(strBuffer, "%.1f %.1fs", count/elapse_time, elapse_time);
		u8g2_DrawStr(&U8g2Handler, 0, 16, strBuffer);
		sprintf(strBuffer, "%08d", count);
		u8g2_DrawStr(&U8g2Handler, 0, 32, strBuffer);
		u8g2_SendBuffer(&U8g2Handler);
		
		count++;
		
		wait_ms(10);

#if (PIN_CHECK)
		CheckPin2.write(0);
#endif
    }
}

