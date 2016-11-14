#ifndef _IRRECEIVER_H_
#define _IRRECEIVER_H_

#include <stdbool.h>
#include "IRCode.h"

class IRReceiver {
public:
	IRReceiver(int pin);
	virtual ~IRReceiver();
	void enable();
	void disable();
	void resume();
	IRCode* decode();

private:
	void configurePin();
	void configureTimer();
	void enableInterrupt();
};

#endif
