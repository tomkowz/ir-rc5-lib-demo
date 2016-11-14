#ifndef _IRSENDER_H_
#define _IRSENDER_H_

class IRSender {
public:
	IRSender(int pwmPin);
	virtual ~IRSender();
	void send(unsigned long data, int bitsCount);

private:
	void configurePin();
	void configureTimer(int khzFrequency);
	void disableInterrupt();
	void enable();
	void sendPulse(int microseconds);
	void sendSpace(int microseconds);
	void send0();
	void send1();
	void enablePWM();
	void disablePWM();
	int pin;
	unsigned long int previousValue;
	bool toggleBit;
};

#endif
