#ifndef _MsTimer_3_h
#define _MsTimer_3_h

#include <avr/interrupt.h>

class MsTimer3Class {
public:
	volatile unsigned long msecs;
	void (*func)();
	volatile unsigned long count;
	volatile char overflowing;
	volatile unsigned int tcnt3;
	
	void _overflow();
	
public:
	MsTimer3Class(){};
	void set(unsigned long ms, void (*f)());
	void setMicros(unsigned long us, void (*f)());	
	void start();
	void stop();
};

extern MsTimer3Class MsTimer3;

#endif
