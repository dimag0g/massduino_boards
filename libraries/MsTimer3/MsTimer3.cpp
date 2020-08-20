/*
  MsTimer3.h - Using timer3 with 1ms or 1us resolution
  
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
#include <MsTimer3.h>

//#if defined (__MD328P__)

void MsTimer3Class::set(unsigned long ms, void (*f)()) {
	float prescaler = 0.0;
	
	TIFR3 = 0xff;
	TIMSK3 = 0;
	TCCR3A &= ~((1<<WGM31) | (1<<WGM30));
	TCCR3B &= ~((1<<WGM32) | (1 << WGM33));
	
	if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR3B &= ~((1<<CS32) | (1<<CS30));
		TCCR3B |= (1<<CS31);
		prescaler = 8.0;
	}
	else //if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) 
	{	// prescaler set to 64
		TCCR3B &= ~(1<<CS32);
		TCCR3B |= (1<<CS31) | (1<<CS30);
		prescaler = 64.0;
	} 	
	
	tcnt3 = 65536U - (int)((float)F_CPU * 0.001 / prescaler);
	
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

void MsTimer3Class::setMicros(unsigned long us, void (*f)()) {
	float prescaler = 0.0;
	
	TIMSK3 = 0;
	TCCR3A &= ~((1<<WGM31) | (1<<WGM30));
	TCCR3B &= ~((1<<WGM32) | (1 << WGM33));
	
	//prescaler set to 1
	TCCR3B &= ~((1<<CS32) | (1<<CS31));
	TCCR3B |= (1<<CS30);
	prescaler = 1.0;

	if(us > 4095u)
		us = 4095u;

	tcnt3 = 65536U - (int)((float)F_CPU * 0.000001 * us / prescaler);
	msecs = 1;
		
	func = f;
}

void MsTimer3Class::start() {
	count = 0;
	overflowing = 0;

	TCNT3H = tcnt3 >> 8;
	TCNT3L = tcnt3 & 0xff;
	TIFR3 |= _BV(TOV3);
	TIMSK3 |= _BV(TOIE3);
}

void MsTimer3Class::stop() {
	TIMSK3 &= ~(_BV(TOIE3));
}

void MsTimer3Class::_overflow() {
	count += 1;
	
	if (count >= msecs && !overflowing) {
		overflowing = 1;
		count = 0;
		func();
		overflowing = 0;
	}
}

#if defined(__AVR_ATmega2560__)
ISR(_VECTOR(35)) {
#else
ISR(_VECTOR(29)) {
#endif
	TCNT3H = MsTimer3.tcnt3 >> 8;
	TCNT3L = MsTimer3.tcnt3 & 0xff;
	TIFR3 |= _BV(TOV3);
	MsTimer3._overflow();
}

MsTimer3Class MsTimer3;

//#endif
