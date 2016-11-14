#include "IRSender.h"

#include "Arduino.h"
#include <avr/interrupt.h>

#include "IRConstants.h"

#define TOPBIT 0x400
#define HALF_BIT_TIME 889

IRSender::IRSender(int pwmPin) {
	this->pin = pwmPin;
	this->toggleBit = 0;
	this->previousValue = -1;
}

IRSender::~IRSender() {}

void IRSender::configurePin() {
	pinMode(this->pin, OUTPUT);
}

void IRSender::configureTimer(int khzFrequency) {
	// Initialize Timer2
	TCCR2A = 0;
	TCCR2B = 0;
	TIMSK2 = 0;

	// Set mode 5, PWM phase correct mode, counts up and down
	bitSet(TCCR2A, WGM20);
	bitSet(TCCR2B, WGM22);

	// Set up 1 prescale
	bitSet(TCCR2B, CS20);

	// Set frequency 36kHz
	OCR2A = 222;
	OCR2B = 111;
}

void IRSender::enable() {
	this->configurePin();
	digitalWrite(this->pin, LOW);
	this->configureTimer(36);
}

void IRSender::enablePWM() {
	bitSet(TCCR2A, COM2B1);
}

void IRSender::disablePWM() {
	bitClear(TCCR2A, COM2B1);
}

void IRSender::send0() {
	this->sendPulse(HALF_BIT_TIME);
	this->sendSpace(HALF_BIT_TIME);
}

void IRSender::send1() {
	this->sendSpace(HALF_BIT_TIME);
	this->sendPulse(HALF_BIT_TIME);
}

// bitsCount max = 11.
void IRSender::send(unsigned long int data, int bitsCount) {
	this->enable();
	data <<= (11 - bitsCount);

	// Send start bits
	this->send1();
	this->send1();

	// Check if we need to toggle a 3rd bit
	if (this->previousValue != data) {
		this->toggleBit = !this->toggleBit;
	}

	// Send toggle bit
	(this->toggleBit == true) ? this->send0() : this->send1();

	// Send the data
	for (int i = 0; i < bitsCount; i++) {
		(data & TOPBIT) ? this->send1() : this->send0();
		data <<= 1;
	}

	this->sendSpace(0); // turn off.
	Serial.println("Signal sent");
}

void IRSender::sendPulse(int microseconds) {
	this->enablePWM();
	delayMicroseconds(microseconds);
}

void IRSender::sendSpace(int microseconds) {
	this->disablePWM();
	delayMicroseconds(microseconds);
}
