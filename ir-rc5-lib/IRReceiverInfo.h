#ifndef _IRRECEIVERINFO_H_
#define _IRRECEIVERINFO_H_

#include "IRConstants.h"
#include "IRReceiverState.h"

/// Size of the buffer inside IRReceiverInfo.
#define IRRECEIVERINFO_BUFFER_SIZE 100

struct IRReceiverInfo {
public:
	/// Pin used as receiver input.
	uint8_t receiverPin;
	/// Current state of receiver;
	IRReceiverState state;
	/// Elapsed ticks (clock cycles) for current measurement. It measures time of space or pulse.
	unsigned int ticksElapsed;
	/// Buffer used to store measurements elapsed times;
	unsigned long int buffer[IRRECEIVERINFO_BUFFER_SIZE];
	/// Length of the buffer used.
	unsigned int bufferLength;
};

#endif
