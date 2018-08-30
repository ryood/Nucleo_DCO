/*
   Nucleo DCO Test03

   DDS Test
   
   2018.08.28

*/
#include "mbed.h"

#include "wavetable_12bit_32k.h"
#define COUNT_OF_ENTRIES  (32768)

#define PIN_CHECK   (1)
#define UART_TRACE  (0)
#define TITLE_STR1  ("Nucleo DCO Test03")
#define TITLE_STR2  ("2018.08.28")

// Pin Assign
AnalogOut Dac1(PA_4);
AnalogOut Dac2(PA_5);

#if (PIN_CHECK)
DigitalOut CheckPin1(D4);
DigitalOut CheckPin2(D5);
#endif

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
void InternalDacWrite(int ch, uint16_t val)
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
    
    InternalDacWrite(1, sin_12bit_32k[idx]);
    //InternalDacWrite(2, sin_12bit_32k[idx]);

#if (PIN_CHECK)
    CheckPin1.write(0);
#endif
}

//-------------------------------------------------------------------------------------------------
// Main routine
//

int main()
{
#if (UART_TRACE)
	printf("%s\r\n", TITLE_STR1);
	printf("%s\r\n", TITLE_STR2);
#endif

    tword_m = pow(2.0, 32) * drate / refclk;  // calculate DDS tuning word;
    
    // 1.0s / 1.0us = 1000000.0
    float interruptPeriodUs = 1000000.0 / refclk; 
    ticker.attach_us(&update, interruptPeriodUs);
    
    while (1) {
#if (PIN_CHECK)
		CheckPin2.write(1);
		wait_us(1);
		CheckPin2.write(0);
		wait_us(1);
#endif
    }
}

