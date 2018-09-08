/*
 * InterruptIn Test 02
 * Multi InterruptIn (F446RE)
 *
 * 2018.09.08
 *
 */

#include "mbed.h"

#define TITLE_STR1  ("InterruptIn Test02")
#define TITLE_STR2  (__DATE__)
#define TITLE_STR3  (__TIME__)

#define DEBOUNCE_DELAY  (0.01f)  // sec

DigitalOut led(LED1);

DigitalOut CheckPin1(PA_7);
DigitalOut CheckPin2(PA_6);

// Interrupt: cannot use same pin number
//  not use PB_10 (D6)
InterruptIn Button0(PA_10, PullUp);
InterruptIn Button1(PB_3, PullUp);
InterruptIn Button2(PB_5, PullUp);
InterruptIn Button3(PB_4, PullUp);
InterruptIn Button4(PA_8, PullUp);
InterruptIn Button5(PA_9, PullUp);
InterruptIn Button6(PC_7, PullUp);
InterruptIn Button7(PB_6, PullUp);

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
void debounce4() { if (Button4.read() == 0) { isButtonPushed4 = true; } debouncer4.detach(); }
void debounce5() { if (Button5.read() == 0) { isButtonPushed5 = true; } debouncer5.detach(); }
void debounce6() { if (Button6.read() == 0) { isButtonPushed6 = true; } debouncer6.detach(); }
void debounce7() { if (Button7.read() == 0) { isButtonPushed7 = true; } debouncer7.detach(); }

void interruptHandler0() {
	CheckPin1.write(1);
	debouncer0.attach(&debounce0, DEBOUNCE_DELAY);
	CheckPin1.write(0);
}
void interruptHandler1() { debouncer1.attach(&debounce1, DEBOUNCE_DELAY); }
void interruptHandler2() { debouncer2.attach(&debounce2, DEBOUNCE_DELAY); }
void interruptHandler3() { debouncer3.attach(&debounce3, DEBOUNCE_DELAY); }
void interruptHandler4() { debouncer4.attach(&debounce4, DEBOUNCE_DELAY); }
void interruptHandler5() { debouncer5.attach(&debounce5, DEBOUNCE_DELAY); }
void interruptHandler6() { debouncer6.attach(&debounce6, DEBOUNCE_DELAY); }
void interruptHandler7() { debouncer7.attach(&debounce7, DEBOUNCE_DELAY); }

int main()
{
	printf("\r\n%s\r\n", TITLE_STR1);
	printf("%s %s\r\n", TITLE_STR2, TITLE_STR3);

	Button0.fall(&interruptHandler0);
	Button1.fall(&interruptHandler1);
	Button2.fall(&interruptHandler2);
	Button3.fall(&interruptHandler3);
	Button4.fall(&interruptHandler4);
	Button5.fall(&interruptHandler5);
	Button6.fall(&interruptHandler6);
	Button7.fall(&interruptHandler7);
	
	while(1) {
		led = !led;
		
		if (isButtonPushed0) { printf("Button0 pushed\r\n"); isButtonPushed0 = false; }
		if (isButtonPushed1) { printf("Button1 pushed\r\n"); isButtonPushed1 = false; }
		if (isButtonPushed2) { printf("Button2 pushed\r\n"); isButtonPushed2 = false; }
		if (isButtonPushed3) { printf("Button3 pushed\r\n"); isButtonPushed3 = false; }
		if (isButtonPushed4) { printf("Button4 pushed\r\n"); isButtonPushed4 = false; }
		if (isButtonPushed5) { printf("Button5 pushed\r\n"); isButtonPushed5 = false; }
		if (isButtonPushed6) { printf("Button6 pushed\r\n"); isButtonPushed6 = false; }
		if (isButtonPushed7) { printf("Button7 pushed\r\n"); isButtonPushed7 = false; }
		
		wait(0.2);
	}
}
