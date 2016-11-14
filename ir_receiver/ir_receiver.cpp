// Do not remove the include below
#include "ir_receiver.h"
#include "../ir-rc5-lib/IRReceiver.h"

#define LED 13
#define IR_RECEIVER 3

IRReceiver receiver(IR_RECEIVER);

int blinksCount = 0;

void setup() {
	Serial.begin(9600);
	pinMode(LED, OUTPUT);
	receiver.enable();
}

void decreaseBlinks() {
	blinksCount = max(blinksCount - 1, 0);
}

void increaseBlinks() {
	blinksCount = min(blinksCount + 1, 10);
}


void blink() {
	const int interval = 200;
	delay(interval);
	digitalWrite(LED, HIGH);
	delay(interval);
	digitalWrite(LED, LOW);
}

void process(IRCode* code) {
	if (code == nullptr) { return; }
	if (code->command == 0) {
		decreaseBlinks();
	} else if (code->command == 1) {
		increaseBlinks();
	} else if (code->command == 32) {
		for (int i = 0; i < blinksCount; i++) { blink(); }
		blinksCount = 0;
	}
}

void loop() {
	process(receiver.decode());
}
