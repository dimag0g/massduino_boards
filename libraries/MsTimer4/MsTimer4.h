#ifndef _MsTimer_4_h
#define _MsTimer_4_h

#include <avr/interrupt.h>

#if defined(__AVR_ATmega2560__)

class MsTimer4Class {
public:
	volatile unsigned long msecs;
	void (*func)();
	volatile unsigned long count;
	volatile char overflowing;
	volatile unsigned int tcnt4;
	
	void _overflow();
	
public:
	MsTimer4Class(){};
	void set(unsigned long ms, void (*f)());
	void setMicros(unsigned long us, void (*f)());	
	void start();
	void stop();
};

extern MsTimer4Class MsTimer4;

#else
	
#error "no timer4 supported."

#endif

#endif
