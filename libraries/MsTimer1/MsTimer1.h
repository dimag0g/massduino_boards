#ifndef _MsTimer_1_h
#define _MsTimer_1_h

#include <avr/interrupt.h>

class MsTimer1Class {
public:
	volatile unsigned long msecs;
	void (*func)();
	volatile unsigned long count;
	volatile char overflowing;
	volatile unsigned int tcnt1;
	
	void _overflow();
	
public:
	MsTimer1Class(){};
	void set(unsigned long ms, void (*f)());
	void setMicros(unsigned long us, void (*f)());	
	void start();
	void stop();
};

extern MsTimer1Class MsTimer1;

#endif
