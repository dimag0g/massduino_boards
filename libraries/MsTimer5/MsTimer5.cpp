/*
  MsTimer5.h - Using timer5 with 1ms or 1us resolution
  
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
#include <MsTimer5.h>

#if defined(__AVR_ATmega2560__)

void MsTimer5Class::set(unsigned long ms, void (*f)()) {
	float prescaler = 0.0;
	
	TIFR5 = 0xff;
	TIMSK5 = 0;
	TCCR5A &= ~((1<<WGM51) | (1<<WGM50));
	TCCR5B &= ~((1<<WGM52) | (1 << WGM53));
	
	if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR5B &= ~((1<<CS52) | (1<<CS50));
		TCCR5B |= (1<<CS51);
		prescaler = 8.0;
	}
	else //if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) 
	{	// prescaler set to 64
		TCCR5B &= ~(1<<CS52);
		TCCR5B |= (1<<CS51) | (1<<CS50);
		prescaler = 64.0;
	} 	
	
	tcnt5 = 65536U - (int)((float)F_CPU * 0.001 / prescaler);
	
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

void MsTimer5Class::setMicros(unsigned long us, void (*f)()) {
	float prescaler = 0.0;
	
	TIMSK5 = 0;
	TCCR5A &= ~((1<<WGM51) | (1<<WGM50));
	TCCR5B &= ~((1<<WGM52) | (1 << WGM53));
	
	//prescaler set to 1
	TCCR5B &= ~((1<<CS52) | (1<<CS51));
	TCCR5B |= (1<<CS50);
	prescaler = 1.0;

	if(us > 4095u)
		us = 4095u;

	tcnt5 = 65536U - (int)((float)F_CPU * 0.000001 * us / prescaler);
	msecs = 1;
		
	func = f;
}

void MsTimer5Class::start() {
	count = 0;
	overflowing = 0;

	TCNT5H = tcnt5 >> 8;
	TCNT5L = tcnt5 & 0xff;
	TIFR5 |= _BV(TOV5);
	TIMSK5 |= _BV(TOIE5);
}

void MsTimer5Class::stop() {
	TIMSK5 &= ~(_BV(TOIE5));
}

void MsTimer5Class::_overflow() {
	count += 1;
	
	if (count >= msecs && !overflowing) {
		overflowing = 1;
		count = 0;
		func();
		overflowing = 0;
	}
}


ISR(_VECTOR(50)) {
	TCNT5H = MsTimer5.tcnt5 >> 8;
	TCNT5L = MsTimer5.tcnt5 & 0xff;
	TIFR5 |= _BV(TOV5);
	MsTimer5._overflow();
}

MsTimer5Class MsTimer5;

#endif
