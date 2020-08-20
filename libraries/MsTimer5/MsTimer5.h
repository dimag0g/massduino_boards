#ifndef _MsTimer_5_h
#define _MsTimer_5_h

#include <avr/interrupt.h>

#if defined(__AVR_ATmega2560__)

class MsTimer5Class {
public:
	volatile unsigned long msecs;
	void (*func)();
	volatile unsigned long count;
	volatile char overflowing;
	volatile unsigned int tcnt5;
	
	void _overflow();
	
public:
	MsTimer5Class(){};
	void set(unsigned long ms, void (*f)());
	void setMicros(unsigned long us, void (*f)());	
	void start();
	void stop();
};

extern MsTimer5Class MsTimer5;

#else
	
#error "no timer4 supported."

#endif

#endif
