/*
  MsTimer4.h - Using timer4 with 1ms or 1us resolution
  
  History:
  	April 10, 2018 by Tony Tan
	August 04, 2018 by Tony Tan, Add Atmega2560 support
	
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/

#include "Arduino.h"
#include <MsTimer4.h>

#if defined(__AVR_ATmega2560__)

void MsTimer4Class::set(unsigned long ms, void (*f)()) {
	float prescaler = 0.0;
	
	TIFR4 = 0xff;
	TIMSK4 = 0;
	TCCR4A &= ~((1<<WGM41) | (1<<WGM40));
	TCCR4B &= ~((1<<WGM42) | (1 << WGM43));
	
	if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR4B &= ~((1<<CS42) | (1<<CS40));
		TCCR4B |= (1<<CS41);
		prescaler = 8.0;
	}
	else //if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) 
	{	// prescaler set to 64
		TCCR4B &= ~(1<<CS42);
		TCCR4B |= (1<<CS41) | (1<<CS40);
		prescaler = 64.0;
	} 	
	
	tcnt4 = 65536U - (int)((float)F_CPU * 0.001 / prescaler);
	
	if(ms == 0)
	{
		msecs = 1;
	}
	else
	{
		msecs = ms;
	}
		
	func = f;
}

void MsTimer4Class::setMicros(unsigned long us, void (*f)()) {
	float prescaler = 0.0;
	
	TIMSK4 = 0;
	TCCR4A &= ~((1<<WGM41) | (1<<WGM40));
	TCCR4B &= ~((1<<WGM42) | (1 << WGM43));
	
	//prescaler set to 1
	TCCR4B &= ~((1<<CS42) | (1<<CS41));
	TCCR4B |= (1<<CS40);
	prescaler = 1.0;

	if(us > 4095u)
		us = 4095u;

	tcnt4 = 65536U - (int)((float)F_CPU * 0.000001 * us / prescaler);
	msecs = 1;
		
	func = f;
}

void MsTimer4Class::start() {
	count = 0;
	overflowing = 0;

	TCNT4H = tcnt4 >> 8;
	TCNT4L = tcnt4 & 0xff;
	TIFR4 |= _BV(TOV4);
	TIMSK4 |= _BV(TOIE4);
}

void MsTimer4Class::stop() {
	TIMSK4 &= ~(_BV(TOIE4));
}

void MsTimer4Class::_overflow() {
	count += 1;
	
	if (count >= msecs && !overflowing) {
		overflowing = 1;
		count = 0;
		func();
		overflowing = 0;
	}
}


ISR(_VECTOR(45)) {
	TCNT4H = MsTimer4.tcnt4 >> 8;
	TCNT4L = MsTimer4.tcnt4 & 0xff;
	TIFR4 |= _BV(TOV4);
	MsTimer4._overflow();
}

MsTimer4Class MsTimer4;

#endif
