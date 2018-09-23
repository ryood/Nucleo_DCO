/*
 * InterruptIn Test 03
 * Multi InterruptIn (F767RE)
 *
 * 2018.09.08
 *
 */

#include "mbed.h"

#define TITLE_STR1  ("InterruptIn Test03")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define DEBOUNCE_DELAY  (10000)  // usec

DigitalOut led(LED1);

DigitalOut CheckPin1(PF_14);  // D4
DigitalOut CheckPin2(PE_11);  // D5

// Interrupt: cannot use same pin number
//  not use PE_10
InterruptIn Button0(PE_8, PullUp);
InterruptIn Button1(PE_7, PullUp);
InterruptIn Button2(PE_10, PullUp);
InterruptIn Button3(PE_12, PullUp);

Ticker debouncer0;
Ticker debouncer1;
Ticker debouncer2;
Ticker debouncer3;

volatile bool isButtonPushed0 = false;
volatile bool isButtonPushed1 = false;
volatile bool isButtonPushed2 = false;
volatile bool isButtonPushed3 = false;

void debounce0() {
	CheckPin2.write(1);
	if (Button0.read() == 0) {
		isButtonPushed0 = true;
	}
	debouncer0.detach();
	CheckPin2.write(0);
}
void debounce1() { if (Button1.read() == 0) { isButtonPushed1 = true; } debouncer1.detach(); }
void debounce2() { if (Button2.read() == 0) { isButtonPushed2 = true; } debouncer2.detach(); }
void debounce3() { if (Button3.read() == 0) { isButtonPushed3 = true; } debouncer3.detach(); }

void interruptHandler0() {
	CheckPin1.write(1);
	debouncer0.attach_us(&debounce0, DEBOUNCE_DELAY);
	CheckPin1.write(0);
}
void interruptHandler1() { debouncer1.attach_us(&debounce1, DEBOUNCE_DELAY); }
void interruptHandler2() { debouncer2.attach_us(&debounce2, DEBOUNCE_DELAY); }
void interruptHandler3() { debouncer3.attach_us(&debounce3, DEBOUNCE_DELAY); }

int main()
{
	printf("\r\n%s\r\n", TITLE_STR1);
	printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);

	Button0.fall(&interruptHandler0);
	Button1.fall(&interruptHandler1);
	Button2.fall(&interruptHandler2);
	Button3.fall(&interruptHandler3);
	
	while(1) {
		led = !led;
		
		if (isButtonPushed0) { printf("Button0 pushed\r\n"); isButtonPushed0 = false; }
		if (isButtonPushed1) { printf("Button1 pushed\r\n"); isButtonPushed1 = false; }
		if (isButtonPushed2) { printf("Button2 pushed\r\n"); isButtonPushed2 = false; }
		if (isButtonPushed3) { printf("Button3 pushed\r\n"); isButtonPushed3 = false; }
		
		wait(0.2);
	}
}
