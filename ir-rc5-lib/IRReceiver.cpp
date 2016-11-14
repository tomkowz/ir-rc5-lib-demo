#include "IRReceiver.h"

#include "Arduino.h"
#include <avr/interrupt.h>

#include "IRConstants.h"
#include "RC5Decoder.h"
#include "IRReceiverInfo.h"
#include "IRSignalType.h"

#define TIMER_FREQUENCY 36000

// 50 milliseconds
#define NO_ACTION_PERIOD_MICROSECONDS 50000

// 27,78 = 28
#define MICROSECONDS_PER_TICK (1 / TIMER_FREQUENCY) * 1000000

/// Ticks required to treat signal as no-action. microseconds / microseconds-per-tick.
/// ~1800 ticks for 36kHz frequency
#define NO_ACTION_PERIOD (TIMER_FREQUENCY / MICROSECONDS_PER_TICK)

static IRReceiverInfo info;

// Public

IRReceiver::IRReceiver(int p) {
	info.receiverPin = p;
}

IRReceiver::~IRReceiver() {}

void IRReceiver::enable() {
	cli();
	configureTimer();
	sei();
	this->resume();
	configurePin();
}

void IRReceiver::disable() {
	TIMSK2 = 0;
}

void IRReceiver::resume() {
	info.state = Idle;
	info.ticksElapsed = 0;
	info.bufferLength = 0;
}

IRCode* IRReceiver::decode() {
	// Be sure that there is no new data coming.
	if (info.state != Stop) { return false; }

	IRCode* code = RC5Decoder::decode(info.buffer, info.bufferLength);
	this->resume();
	return code;
}

// Private

void IRReceiver::configurePin() {
	pinMode(info.receiverPin, INPUT);
}

void IRReceiver::configureTimer() {
	TCCR2A = 0;
	TCCR2B = 0;

	// Set mode 5, PWM phase correct mode, counts up and down
	bitSet(TCCR2A, WGM20);
	bitSet(TCCR2B, WGM22);

	// Set up 1 prescale
	bitSet(TCCR2B, CS20);

	// Set frequency 36kHz
	OCR2A = 222;
	OCR2B = 111;

	// Interrupt mask
	bitSet(TIMSK2, OCIE2A);
}

// ISR

void storeTime() {
	info.buffer[info.bufferLength] = info.ticksElapsed * MICROSECONDS_PER_TICK;
	info.bufferLength += 1;
	info.ticksElapsed = 0;
}

void storeTimeOfSpaceAndExpectPulse() {
	storeTime();
	info.state = ExpectPulse;
}

void storeTimeOfPulseAndExpectSpace() {
	storeTime();
	info.state = ExpectSpace;
}

void storeTimeOfSpaceAndStopReceiving() {
	storeTime();
	info.state = Stop;
}

ISR(TIMER2_COMPA_vect) {
	// Read data from the stream
	unsigned int data = (uint8_t)digitalRead(info.receiverPin);

	// count time
	info.ticksElapsed += 1;

	// Perform sanity checks whether we have a room for new readings.
	if (info.bufferLength >= IRRECEIVERINFO_BUFFER_SIZE) {
//		Serial.println("Buffer overflow.");
		info.state = Stop;
		info.ticksElapsed = 0;
		return;
	}

	if (info.state == Idle) {
		// When in Idle, receiver is expecting pulse signal.
		// Also, we need to wait a required time between
		// signals which is 50ms. The no-signal period will
		// be around 114ms but the hardware is not that accurate.
		if (info.ticksElapsed < NO_ACTION_PERIOD || data != Pulse) { return; }
		// Otherwise...
		storeTimeOfSpaceAndExpectPulse();
	} else if (info.state == ExpectPulse) {
		// Measure time till space shows up in the stream.
		// Save it and start expecting space.
		if (data != Space) { return; }
		storeTimeOfPulseAndExpectSpace();
	} else if (info.state == ExpectSpace) {
		// Measure time till pulse shows up.
		// Save it and expect pulse.
		// Also, we need additional check to know whether
		// space is getting bigger than NO_ACTION_PERIOD
		// and then we must stop receiving data.
		if (data == Pulse) {
			storeTimeOfSpaceAndExpectPulse();
		} else if (info.ticksElapsed > NO_ACTION_PERIOD) {
			storeTimeOfSpaceAndStopReceiving();
		}
	}
}
