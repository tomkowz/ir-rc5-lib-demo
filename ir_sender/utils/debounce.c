//#include "pins_arduino.h"
#ifndef _DEBOUNCE_C_
#define _DEBOUNCE_C_

#include "stdint.h"

void delay(unsigned long miliseconds);
int digitalRead(uint8_t pin);

void debounce(int pin, int expectedState, int miliseconds, void (*callbackFunction)(void)) {
	delay(miliseconds);
	if (digitalRead(pin) != expectedState) { return; }
	callbackFunction();
	while (digitalRead(pin) == expectedState);
}

#endif
