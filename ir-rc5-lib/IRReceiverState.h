#ifndef _IRRECEIVERSTATE_H_
#define _IRRECEIVERSTATE_H_

enum IRReceiverState {
	/// Nothing is going on. Actually receiver measure time of space.
	Idle,
	/// Receiver is measuring time of pulse.
	ExpectPulse,
	/// Receiver is measuring time of space.
	ExpectSpace,
	/// Receiver did stop gathering incoming data.
	Stop
};

#endif
