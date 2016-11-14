#include "Arduino.h"
#include "IRConstants.h"
#include "IRSignalType.h"
#include "RC5Decoder.h"
#include "IRCode.h"

/// Number of half bits that need to be decoded.
#define HALF_BITS_TO_DECODE 28

/// Some threshold of how much duration of a recorded signal can differ.
#define THRESHOLD 150

bool isValidHalfBit(unsigned long int duration) {
	return (duration >= HALF_BIT_TIME - THRESHOLD) && (duration <= HALF_BIT_TIME + THRESHOLD);
}

bool isValidFullBit(unsigned long int duration) {
	return (duration >= (2 * HALF_BIT_TIME) - THRESHOLD) && (duration <= (2 * HALF_BIT_TIME) + THRESHOLD);
}

bool decodeReading(unsigned long int duration, unsigned int& halfBitsCount) {
	if (isValidHalfBit(duration)) {
		halfBitsCount = 1;
		return true;
	}

	if (isValidFullBit(duration)) {
		halfBitsCount = 2;
		return true;
	}
	// duration of this reading is wrong.
	return false;
}

IRCode* RC5Decoder::decode(unsigned long int* buffer, unsigned int bufferLength) {
	// In order to decode signal correctly we need to collect
	// 14 bits which is 28 half bits.
	// 2 start bits, 1 toggle bit, 5 address bits, 6 command bits.

	unsigned int halfBits[HALF_BITS_TO_DECODE];
	unsigned int halfBitsIndex = 0;

//	// We need to keep track of time during decoding
//	unsigned long int time = 0;

	// Before iterating we need to set first half bit as space
	halfBits[halfBitsIndex] = Space;
	halfBitsIndex += 1;

	bool success;

	for (unsigned int i = 1; i < bufferLength; i++) {
		// Calculate next half bits value
		IRSignalType readingValue = halfBits[halfBitsIndex - 1] == Space ? Pulse : Space;
		// Get current reading
		unsigned long int readingDuration = buffer[i];

		// If this is last reading from the buffer, it might be space
		// and it might get very long in time. We do not want to decode
		// all the duration so we trim it to be a full bit at most.
		if (readingDuration > (2 * HALF_BIT_TIME) + THRESHOLD && (i == bufferLength - 1)) {
			readingDuration = (2 * HALF_BIT_TIME);
		}

		unsigned int halfBitsFromReading = 0;
		if (decodeReading(readingDuration, halfBitsFromReading) == true) {
			unsigned int availableSpace = HALF_BITS_TO_DECODE - halfBitsIndex;
			unsigned int numberOfHalfBitsToAppend = halfBitsFromReading < availableSpace ? halfBitsFromReading : availableSpace;
			for (unsigned int j = 0; j < numberOfHalfBitsToAppend; j++) {
				halfBits[halfBitsIndex++] = readingValue;
			}
		} else {
			Serial.println("Decoding failed");
			success = false;
			break;
		}

//		// Update `time`
//		time += readingDuration;

		if (halfBitsIndex == HALF_BITS_TO_DECODE) {
//			Serial.println("Decoded required number of half bits");
			success = true;
			break;
		}
	}

	if (success == false) {
		return nullptr;
	}

	// Convert half bits into bits
	unsigned int bits[HALF_BITS_TO_DECODE / 2];
	for (unsigned int i = 0; i < HALF_BITS_TO_DECODE; i += 2) {
		bits[i / 2] = (halfBits[i] == Space && halfBits[i + 1] == Pulse) ? 1 : 0;
//		Serial.print(bits[i / 2]);
//		Serial.print(" ");
	}
//	Serial.println();

	// Check whether first two bits are start bits
	if (bits[0] != 1 || bits[1] != 1) {
		Serial.println("Wrong start bits");
		return nullptr;
	}

	// Prepare results
	IRCode* code = new IRCode();

	// Set toggle bit
	code->toggle = bits[2];

	// Set address value
	code->address = 0;
	int offset = 3;
	int bitsToSet = 5;
	for (int i = offset; i <= offset + bitsToSet; i++) {
		int bitValue = bits[i];
		int bitIndex = bitsToSet - (i - offset);
		code->address |= bitValue << (bitIndex);
	}

	// Set command value
	code->command = 0;
	offset = 8;
	bitsToSet = 5;
	for (int i = offset; i <= offset + bitsToSet; i++) {
		int bitValue = bits[i];
		int bitIndex = bitsToSet - (i - offset);
		code->command |= bitValue << (bitIndex);
	}

	Serial.print("<IRCode> toggle: ");
	Serial.print(code->toggle);
	Serial.print(", address: ");
	Serial.print(code->address);
	Serial.print(", command: ");
	Serial.println(code->command);

	return code;
}
