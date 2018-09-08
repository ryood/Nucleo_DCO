#include "mbed.h"

#define DEBOUNCE_DELAY  (0.01f)  // sec

InterruptIn Button(D2);
DigitalOut led(LED1);

Ticker debouncer;

volatile bool isButtonPushed = false;

void debounce()
{
	if (Button.read() == 0) {
		led = !led;
		isButtonPushed = true;
	}
	debouncer.detach();
}

void interruptHandler()
{
	debouncer.attach(&debounce, DEBOUNCE_DELAY);
}

int main()
{
	printf("\r\nInterruptIn Test01\r\n");

	Button.mode(PullUp);
	Button.fall(&interruptHandler);
	
	while(1) {
		if (isButtonPushed) {
			printf("Button pushed\r\n");
			isButtonPushed = false;
		}
	}
}
