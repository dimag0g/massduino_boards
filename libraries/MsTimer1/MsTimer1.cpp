/*
  MsTimer1.h - Using timer1 with 1ms or 1us resolution
  
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
#include <MsTimer1.h>

//#if defined (__MD328P__)

void MsTimer1Class::set(unsigned long ms, void (*f)()) {
	float prescaler = 0.0;
	
	TIFR1 = 0xff;
	TIMSK1 = 0;
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));
	TCCR1B &= ~((1<<WGM12) | (1 << WGM13));
	
	if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR1B &= ~((1<<CS12) | (1<<CS10));
		TCCR1B |= (1<<CS11);
		prescaler = 8.0;
	}
	else //if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) 
	{	// prescaler set to 64
		TCCR1B &= ~(1<<CS12);
		TCCR1B |= (1<<CS11) | (1<<CS10);
		prescaler = 64.0;
	} 	
	
	tcnt1 = 65536U - (int)((float)F_CPU * 0.001 / prescaler);
	
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

void MsTimer1Class::setMicros(unsigned long us, void (*f)()) {
	float prescaler = 0.0;
	
	TIMSK1 = 0;
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));
	TCCR1B &= ~((1<<WGM12) | (1 << WGM13));
	
	//prescaler set to 1
	TCCR1B &= ~((1<<CS12) | (1<<CS11));
	TCCR1B |= (1<<CS10);
	prescaler = 1.0;

	if(us > 4095u)
		us = 4095u;

	tcnt1 = 65536U - (int)((float)F_CPU * 0.000001 * us / prescaler);
	msecs = 1;
		
	func = f;
}

void MsTimer1Class::start() {
	count = 0;
	overflowing = 0;

	TCNT1H = tcnt1 >> 8;
	TCNT1L = tcnt1 & 0xff;
	TIFR1 |= _BV(TOV1);
	TIMSK1 |= _BV(TOIE1);
}

void MsTimer1Class::stop() {
	TIMSK1 &= ~(_BV(TOIE1));
}

void MsTimer1Class::_overflow() {
	count += 1;
	
	if (count >= msecs && !overflowing) {
		overflowing = 1;
		count = 0;
		func();
		overflowing = 0;
	}
}

#if defined(__AVR_ATmega2560__)
ISR(_VECTOR(20)) {
#else
ISR(_VECTOR(13)) {
#endif
	TCNT1H = MsTimer1.tcnt1 >> 8;
	TCNT1L = MsTimer1.tcnt1 & 0xff;
	TIFR1 |= _BV(TOV1);
	MsTimer1._overflow();
}

MsTimer1Class MsTimer1;

//#endif
