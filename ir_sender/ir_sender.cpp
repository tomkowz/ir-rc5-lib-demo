#include "ir_sender.h"

#include "utils/block.h"
#include "utils/debounce.c"
#include "../ir-rc5-lib/IRSender.h"

#define IR_SENDER 3
#define SWITCH_CONFIRM 8
#define SWITCH_INCREASE 9
#define SWITCH_DECREASE 10

IRSender sender(IR_SENDER);

void setup() {
	Serial.begin(9600);
	pinMode(SWITCH_CONFIRM, INPUT_PULLUP);
	pinMode(SWITCH_INCREASE, INPUT_PULLUP);
	pinMode(SWITCH_DECREASE, INPUT_PULLUP);
}

void inverseState(int pin) {
	digitalWrite(pin, !digitalRead(pin));
}

void processDecrease() {
	debounce(SWITCH_DECREASE, LOW, 20, block(
		Serial.println("Sending Decrease");
		sender.send(0, 11);
	));
}

void processIncrease() {
	debounce(SWITCH_INCREASE, LOW, 20, block(
		Serial.println("Sending Increase");
		sender.send(1, 11);
	));
}

void processConfirm() {
	debounce(SWITCH_CONFIRM, LOW, 20, block(
		Serial.println("Sending Confirm");
		sender.send(32, 11);
	));
}

void loop() {
	processDecrease();
	processIncrease();
	processConfirm();
}
