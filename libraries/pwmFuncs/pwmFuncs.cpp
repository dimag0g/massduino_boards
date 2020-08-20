/*
  _pwmFuncs_h.h - extend pwm functions for mega2560
  
  History:
	August 17, 2018 by Tony Tan
	
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
#include <pwmFuncs.h>
#include "wiring_private.h"
#include "pins_arduino.h"


#if defined(__MD328D__) || defined(__MD328P__)

// PWM working mode
// Function:
//	wmode: pwm working mode
//		- PWM_MODE_NORMAL: normal single output
//		- PWM_MODE_COMPM0: complementary dual output 
//		- PWM_MODE_COMPM1: complementary dual output (inverted)
//		- PWM_MODE_COMPM2: complementary dual output 
//		- PWM_MODE_COMPM3: complementary dual output (inverted)
//	fmode: pwm frequency settings
//		- PWM_FREQ_SLOW: slow range
//		- PWM_FREQ_NORMAL: normal range
//		- PWM_FREQ_FAST: fast range 
//		- PWM_FREQ_BOOST: boost target frequency by x4
//	dband: dead band settings
//		- only valid for complementary working mode 
// note:
//		- Timer 2 is used for system tick, so don't touch!!
//static unsigned char tmr1_boost_en = 0;
//static unsigned char tmr3_boost_en = 0;

void pwmMode(unsigned char pin, unsigned char wmode, unsigned char fmode, unsigned char dband)
{
	volatile unsigned char *pTCCRX = 0;

	unsigned char timer = digitalPinToTimer(pin) & 0xf0;

	if(timer == TIMER0) { // TIMER0
		pTCCRX = &TCCR0B;
		if(wmode == PWM_MODE_NORMAL) {
			cbi(TCCR0B, DTEN0);
			cbi(TCCR0A, COM0B0);
		} else {
			sbi(TCCR0B, DTEN0);
			TCCR0A = (TCCR0A & ~_BV(COM0B0)) | (wmode & 0x10);
			DTR0 = ((dband & 0xf) << 4) | (dband & 0xf);
		}

		if((fmode & PWM_FREQ_BOOST) == PWM_FREQ_BOOST) {
			// enable frequency boost (x4) mode
			sbi(TCKCSR, F2XEN);
			delayMicroseconds(10);
			sbi(TCKCSR, TC2XS0);					
		} else if(bit_is_set(TCKCSR, TC2XS0)) {
			cbi(TCKCSR, TC2XS0);
			delayMicroseconds(10);
			cbi(TCKCSR, F2XEN);				
		}
	} else if(timer == TIMER1) { // TIMER1
		pTCCRX = &TCCR1B;
		if(wmode == PWM_MODE_NORMAL) {
			cbi(TCCR1C, DTEN1);
			cbi(TCCR1A, COM1B0);
		} else {
			sbi(TCCR1C, DTEN1);
			TCCR1A = (TCCR1A & ~_BV(COM1B0)) | (wmode & 0x10);
            #if defined(__MD328P__)
			DTR1L = dband;
			DTR1H = dband;
            #endif
            #if defined(__MD328D__)
            DTR1 = dband;
            #endif
		}
		if((fmode & PWM_FREQ_BOOST) == PWM_FREQ_BOOST) {
			sbi(TCKCSR, F2XEN);
			delayMicroseconds(10);
			sbi(TCKCSR, TC2XS1);
		} else if(bit_is_set(TCKCSR, TC2XS1)) {
			cbi(TCKCSR, TC2XS1);
			delayMicroseconds(10);
			cbi(TCKCSR, F2XEN);
		}		
	} 
    #if defined(__MD328P__)
    else if(timer == TIMER3) { // TIMER3
		pTCCRX = &TCCR3B;
		if(wmode == PWM_MODE_NORMAL) {
			cbi(TCCR3C, DTEN3);
			cbi(TCCR3A, COM3B0);
		} else {
			sbi(TCCR3C, DTEN3);
			TCCR3A = (TCCR3A & ~_BV(COM3B0)) | (wmode & 0x10);
			DTR3A = dband;
			DTR3B = dband;
		}
	}
    #endif

	if(pTCCRX == 0) return;

	if((fmode & 0x7f) == PWM_FREQ_SLOW) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_SLOW;	// prescale = 1024 (slowest mode)
	} else if((fmode & 0x7f) == PWM_FREQ_FAST) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_FAST; // prescale = 1 (fastest mode)
	} else if ((fmode & 0x7f) == PWM_FREQ_NORMAL) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_NORMAL;	// prescale = 64 (default)
	}
}

// enhanced PWM settings
// Function:
//	- set PWM frequency (unit: Hz), return maximum duty cycle 
// Note: 
//	- only PWM Timer1/Timer3 support frequency update
unsigned int pwmFrequency(unsigned char pin, unsigned long fhz)
{
	unsigned long icrx = 0;
	unsigned char csxs = 0;
	unsigned char boost = 0;
	volatile unsigned char *pICRX = 0;
    unsigned long maxFreq = F_CPU;

	unsigned char timer = digitalPinToTimer(pin) & 0xf0;

	// Note for TIMER0 
	// ============================================================================
	// timer 0 working in FPWM mode which TOP is fixed to 0xFF
	// so we can change its prescale to set frequency range (fast/normal/slow)
	// fast mode:	16000000/(1*256) = 62.5K, support boost up to 62.5x4 = 250KHz
	// normal mode:	16000000/(64*256) = 976Hz, support boost up to 3.9KHz
	// slow mode:	16000000/(1024*256) = 61Hz, support boost up to 244Hz
	// ============================================================================

	if(timer == TIMER1) { // TIMER1
		pICRX = &ICR1L;
		csxs = TCCR1B & 0x7;
		boost = bit_is_set(TCKCSR, TC2XF1);
	} 
    #if defined(__MD328P__)
    else if(timer == TIMER3) { // TIMER3
		pICRX = &ICR3L;
		csxs = TCCR3B & 0x7;
	}
    #endif

	if(pICRX == 0) return 0xff;

    if(boost)
    {
        maxFreq <<= 2;
    }

    switch( csxs )
    {
    case 1:
        break;
    case 2:
        maxFreq >>= 3;
        break;
    case 3:
        maxFreq >>= 6;
        break;
    case 4:
        maxFreq >>= 8;
        break;
    case 5:
        maxFreq >>= 10;
        break;
    }

    icrx = (maxFreq / fhz);
    if(icrx >= 65535u)
    {
        icrx = 65535u;
    }

//  // DO NOT try to merge the two cases, compiler will try to
//  // optimize the divider if either of oprands is constant value
//  if(boost == 0) {
//  	if(csxs == PWM_FREQ_FAST) { // fast mode
//  		icrx = (unsigned int) ((F_CPU >> 1) / fhz);
//  	} else if(csxs == PWM_FREQ_NORMAL) { // normal mode
//  		icrx = (unsigned int) ((F_CPU >> 7) / fhz);
//  	} else if(csxs == PWM_FREQ_SLOW) { // slow mode
//  		icrx = (unsigned int) ((F_CPU >> 11) / fhz);
//  	}
//  } else {
//  	if(csxs == PWM_FREQ_FAST) { // fast mode
//  		icrx = (unsigned int) ((64000000UL >> 1) / fhz);
//  	} else if(csxs == PWM_FREQ_NORMAL) { // normal mode
//  		icrx = (unsigned int) ((64000000UL >> 7) / fhz);
//  	} else if(csxs == PWM_FREQ_SLOW) { // slow mode
//  		icrx = (unsigned int) ((64000000UL >> 11) / fhz);
//  	}
//  }
	
	atomicWriteWord(pICRX, (unsigned int)icrx);

	return icrx;
}

// Function:
//	- return frequency (in Hz) by give PWM resolution (bits width of duty)
// Note: 
//	- timer0/2 works in FPWM mode, pwm frequency is fixed by given mode
//	- timer1/3 works in PCPWM mode, means frequency reduced by a half
unsigned long pwmResolution(unsigned char pin, unsigned char resBits)
{
	unsigned char csxs = 0;
	unsigned char boost = 0;
	unsigned long freq = 0x0UL;
    unsigned long maxFreq = F_CPU;
    unsigned long bitMask;

	unsigned char timer = digitalPinToTimer(pin) & 0xf0;

	if(timer != TIMER1 && timer != TIMER3)
		return 0x0UL;

	if(timer == TIMER1) { // TIMER1
		csxs = TCCR1B & 0x7;
		boost = bit_is_set(TCKCSR, TC2XF1);
	} 
    #if defined(__MD328P__)
    else if(timer == TIMER3) { // TIMER3
		csxs = TCCR3B & 0x7;
	}	
    #endif
	
    if(boost)
    {
        maxFreq <<= 2;
    }

    switch( csxs )
    {
    case 1:
        break;
    case 2:
        maxFreq >>= 3;
        break;
    case 3:
        maxFreq >>= 6;
        break;
    case 4:
        maxFreq >>= 8;
        break;
    case 5:
        maxFreq >>= 10;
        break;
    }

//  if(boost != 0) {
//  	if(csxs == PWM_FREQ_FAST) {
//  		freq = (64000000UL >> 1) / (1 << resBits);
//  	} else if(csxs == PWM_FREQ_SLOW) {
//  		freq = (64000000UL >> 11) / (1 << resBits);
//  	} else { // PWM_FREQ_NORMAL
//  		freq = (64000000UL >> 7) / (1 << resBits);
//  	}
//  } else {
//  	if(csxs == PWM_FREQ_FAST) {
//  		freq = (F_CPU >> 1) / (1 << resBits);
//  	} else if(csxs == PWM_FREQ_SLOW) {
//  		freq = (F_CPU >> 11) / (1 << resBits);
//  	} else { // PWM_FREQ_NORMAL
//  		freq = (F_CPU >> 7) / (1 << resBits);
//  	}
//  }
    
    bitMask = 1;
    bitMask <<= resBits;
    freq = maxFreq / bitMask;

	// update pwm frequency
	pwmFrequency(pin, freq);

	return freq;
}

void pwmWrite(unsigned char pin, unsigned int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.

	// duty cycle validation: settings should not overflow of PWM frequency
	
	switch(digitalPinToTimer(pin)) {
		// XXX fix needed for atmega8
		#if defined(TCCR0) && defined(COM00) && !defined(__AVR_ATmega8__)
		case TIMER0A:
			// connect pwm to pin on timer 0
			sbi(TCCR0, COM00);
			OCR0 = (unsigned char)val; // set pwm duty
			break;
		#endif

		#if defined(TCCR0A) && defined(COM0A1)
		case TIMER0A: // D6
			// connect pwm to pin on timer 0, channel A
			OCR0A = (unsigned char)val; // set pwm duty
			sbi(TCCR0A, COM0A1);
			cbi(TCCR0B, OC0AS);	//*****
			sbi(DDRD, PD6);
			break;
		#if defined(__MD3248P__)			
		case TIMER0AX: // E4
			OCR0A = (unsigned char)val;						
			sbi(TCCR0A, COM0A1);
			sbi(TCCR0B, OC0AS);	 //*****
			sbi(DDRE, PE4);
			break;
		#endif
		#endif
		#if defined(TCCR0A) && defined(COM0B1)
		case TIMER0B: // D5
			// connect pwm to pin on timer 0, channel B			
			OCR0B = (unsigned char)val; // set pwm duty			
			sbi(TCCR0A, COM0B1);				
			#if defined(__MD3248P__)
			unlockWrite(&PMX0, (PMX0 & ~_BV(C0BF3)));
			#endif					
			sbi(DDRD, PD5);
			break;
		#if defined(__MD3248P__)
		case TIMER0BX: // F3
			OCR0B = (unsigned char)val;
			sbi(TCCR0A, COM0B1);
			unlockWrite(&PMX0, (PMX0 | _BV(C0BF3)));
			sbi(DDRF, PF3);
			break;
		#endif
		#endif

		#if defined(TCCR1A) && defined(COM1A1)
		case TIMER1A: // B1
			// connect pwm to pin on timer 1, channel A
			//OCR1A = val; // set pwm duty
			atomicWriteWord(&OCR1AL, val);
			sbi(TCCR1A, COM1A1);
			#if defined(__MD328P__)	
			unlockWrite(&PMX0, (PMX0 & (~_BV(C1AF5))));
			#endif			
			sbi(DDRB, PB1);
			break;
		#if defined(__MD328P__)
			// F5 for MD3248P/QFP48
			// E5 for MD328P/QFP32 (tied with F5)
		case TIMER1AX: 
        #ifndef __MD3248P__   //if md328p32
            cbi(DDRE, PE5);
        #endif
            //OCR1A = val;
            atomicWriteWord(&OCR1AL, val);
            sbi(TCCR1A, COM1A1);
            unlockWrite(&PMX0, (PMX0 | _BV(C1AF5)));
            sbi(DDRF, PF5);
            break;
  	    #endif
		#endif
		#if defined(TCCR1A) && defined(COM1B1)
		case TIMER1B: // B2
			// connect pwm to pin on timer 1, channel B			
			//OCR1B = val; // set pwm duty
			atomicWriteWord(&OCR1BL, val);
			sbi(TCCR1A, COM1B1);
			#if defined(__MD328P__)
			unlockWrite(&PMX0, (PMX0 & ~_BV(C1BF4)));		
			#endif			
			sbi(DDRB, PB2);
			break;
		
		#if defined(__MD328P__)
		// F4 for MD3248P/QFP48
		// E4 for MD328P/QFP32 (tied with F4)
		case TIMER1BX:	
		#ifndef __MD3248P__   //if md328p32
			cbi(DDRE, PE4);
		#endif
			//OCR1B = val;
			atomicWriteWord(&OCR1BL, val);
			sbi(TCCR1A, COM1B1);			
			unlockWrite(&PMX0, (PMX0 | _BV(C1BF4)));
			sbi(DDRF, PF4);	
			break;
		#endif
		#endif

		#if defined(TCCR2) && defined(COM21)
		case TIMER2:
			// connect pwm to pin on timer 2
			OCR2 = (unsigned char)val; // set pwm duty			
			sbi(TCCR2, COM21);
			break;
		#endif

		#if defined(TCCR2A) && defined(COM2A1)
		case TIMER2A: // B3
			// connect pwm to pin on timer 2, channel A
			OCR2A = (unsigned char)val; // set pwm duty
			sbi(TCCR2A, COM2A1);			
			#if defined(__MD3248P__)
			unlockWrite(&PMX1, (PMX1 & ~_BV(C2AF6)));
			#endif			
			sbi(DDRB, PB3);			
			break;
		#if defined(__MD3248P__)
		case TIMER2AX: // F6
			OCR2A = (unsigned char)val;			
			sbi(TCCR2A, COM2A1);
			unlockWrite(&PMX1, (PMX1 | _BV(C2AF6)));
			sbi(DDRF, PF6);
			break;
		#endif	
		#endif

		#if defined(TCCR2A) && defined(COM2B1)
		case TIMER2B: // D3
			// connect pwm to pin on timer 2, channel B	
			OCR2B = (unsigned char)val; // set pwm duty
			sbi(TCCR2A, COM2B1);
			#if defined(__MD3248P__)
			unlockWrite(&PMX1, (PMX1 & ~_BV(C2BF7)));
			#endif
			sbi(DDRD, PD3);
			break;
		#if defined(__MD3248P__)
		case TIMER2BX: // F7
			OCR2B = (unsigned char)val;
			sbi(TCCR2A, COM2B1);
			unlockWrite(&PMX1, (PMX1 | _BV(C2BF7)));
			sbi(DDRF, PF7);
			break;
		#endif
		#endif

		#if defined(TCCR3A) && defined(COM3A1)
		case TIMER3A: // D1 tied with F1
			// connect pwm to pin on timer 3, channel A
			cbi(UCSR0B, TXEN0);
			cbi(DDRD, PD1);
			atomicWriteWord(&OCR3AL, val);
			sbi(TCCR3A, COM3A1);
			sbi(DDRF, PF1);			
			break;
		#if defined(__MD3248P__)
		case TIMER3AX: // F1 standalone
			atomicWriteWord(&OCR3AL, val);
			sbi(TCCR3A, COM3A1);
			unlockWrite(&PMX1, (PMX1 & ~_BV(C3AC)));
			sbi(DDRF, PF1);
			break;
		case TIMER3AA: // OC3A/ACO standalone
			atomicWriteWord(&OCR3AL, val);
			unlockWrite(&PMX1, (PMX1 | _BV(C3AC)));
			sbi(TCCR3A, COM3A1);
			break;
		#endif	
		#endif

		#if defined(TCCR3A) && defined(COM3B1)
		case TIMER3BX: // F2 tied with D2
		#if defined(__MD328P__)
		case TIMER3B:
			cbi(DDRD, PD2);
		#endif
			// connect pwm to pin on timer 3, channel B
			//OCR3B = val; // set pwm duty
			atomicWriteWord(&OCR3BL, val);
			sbi(TCCR3A, COM3B1);
			sbi(DDRF, PF2);
			break;
		#endif

		#if defined(__MD3248P__)
		#if defined(TCCR3A) && defined(COM3C1)
		case TIMER3C: // F3 
			// connect pwm to pin on timer 3, channel C
			//OCR3C = val; // set pwm duty
			atomicWriteWord(&OCR3CL, val);
			sbi(TCCR3A, COM3C1);
			sbi(DDRF, PF3);
			break;
		#endif
		#endif

		#if defined(TCCR4A)
		case TIMER4A:
			//connect pwm to pin on timer 4, channel A
			sbi(TCCR4A, COM4A1);
			#if defined(COM4A0)		// only used on 32U4
			cbi(TCCR4A, COM4A0);
			#endif
			OCR4A = val;	// set pwm duty
			break;
		#endif
		
		#if defined(TCCR4A) && defined(COM4B1)
		case TIMER4B:
			// connect pwm to pin on timer 4, channel B
			sbi(TCCR4A, COM4B1);
			OCR4B = val; // set pwm duty
			break;
		#endif

		#if defined(TCCR4A) && defined(COM4C1)
		case TIMER4C:
			// connect pwm to pin on timer 4, channel C
			sbi(TCCR4A, COM4C1);
			OCR4C = val; // set pwm duty
			break;
		#endif
			
		#if defined(TCCR4C) && defined(COM4D1)
		case TIMER4D:				
			// connect pwm to pin on timer 4, channel D
			sbi(TCCR4C, COM4D1);
			#if defined(COM4D0)		// only used on 32U4
			cbi(TCCR4C, COM4D0);
			#endif
			OCR4D = val;	// set pwm duty
			break;
		#endif
						
		#if defined(TCCR5A) && defined(COM5A1)
		case TIMER5A:
			// connect pwm to pin on timer 5, channel A
			sbi(TCCR5A, COM5A1);
			OCR5A = val; // set pwm duty
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5B1)
		case TIMER5B:
			// connect pwm to pin on timer 5, channel B
			sbi(TCCR5A, COM5B1);
			OCR5B = val; // set pwm duty
			break;
		#endif

		#if defined(TCCR5A) && defined(COM5C1)
		case TIMER5C:
			// connect pwm to pin on timer 5, channel C
			sbi(TCCR5A, COM5C1);
			OCR5C = val; // set pwm duty
			break;
		#endif
		#if defined(__MD328D__) || defined(__MD328P__)
		case MDDAO0:
			DAL0 = val; 
			break;
		#endif
		#if defined(__MD328D__)
		case MDDAO1:
			DAL1 = val;
			break;
		#endif
		case NOT_ON_TIMER:
		default:
			if (val < 128) {
				digitalWrite(pin, LOW);
			} else {
				digitalWrite(pin, HIGH);
			}
	}
}


#elif defined(__AVR_ATmega2560__)

void pwmAtomicWriteWord(volatile unsigned char *p, unsigned int val)
{
	unsigned char _o_sreg = SREG;

	cli();
	*(p + 1) = (unsigned char)(val >> 8);
	_NOP(); _NOP(); _NOP();
	*p = (unsigned char)val;
	SREG = _o_sreg;
}

//	- For timer0/2 fmode has no effect
void pwmMode(unsigned char pin, unsigned char wmode, unsigned char fmode, unsigned char dband)
{
	volatile unsigned char *pTCCRX = 0;

	unsigned char timer = digitalPinToTimer(pin);

    switch(timer)
    {
    case TIMER0A:
		//pTCCRX = &TCCR0B;
        if( wmode & 0x10 )
            TCCR0A = (TCCR0A | _BV(COM0A0));
        else
            TCCR0A = (TCCR0A & ~_BV(COM0A0));
        break;
    case TIMER0B:
		//pTCCRX = &TCCR0B;
        if( wmode & 0x10 )
            TCCR0A = (TCCR0A | _BV(COM0B0));
        else
            TCCR0A = (TCCR0A & ~_BV(COM0B0));
        break;

    case TIMER1A:
		pTCCRX = &TCCR1B;
        if( wmode & 0x10 )
            TCCR1A = (TCCR1A | _BV(COM1A0));
        else
            TCCR1A = (TCCR1A & ~_BV(COM1A0));
        break;
    case TIMER1B:
		pTCCRX = &TCCR1B;
        if( wmode & 0x10 )
            TCCR1A = (TCCR1A | _BV(COM1B0));
        else
            TCCR1A = (TCCR1A & ~_BV(COM1B0));
        break;

    case TIMER2A:
		//pTCCRX = &TCCR2B;
        if( wmode & 0x10 )
            TCCR2A = (TCCR2A | _BV(COM2A0));
        else
            TCCR2A = (TCCR2A & ~_BV(COM2A0));
        break;
    case TIMER2B:
		//pTCCRX = &TCCR2B;
        if( wmode & 0x10 )
            TCCR2A = (TCCR2A | _BV(COM2B0));
        else
            TCCR2A = (TCCR2A & ~_BV(COM2B0));
        break;

    case TIMER3A:
		pTCCRX = &TCCR3B;
        if( wmode & 0x10 )
            TCCR3A = (TCCR3A | _BV(COM3A0));
        else
            TCCR3A = (TCCR3A & ~_BV(COM3A0));
        break;
    case TIMER3B:
		pTCCRX = &TCCR3B;
        if( wmode & 0x10 )
            TCCR3A = (TCCR3A | _BV(COM3B0));
        else
            TCCR3A = (TCCR3A & ~_BV(COM3B0));
        break;
    case TIMER3C:
		pTCCRX = &TCCR3B;
        if( wmode & 0x10 )
            TCCR3A = (TCCR3A | _BV(COM3C0));
        else
            TCCR3A = (TCCR3A & ~_BV(COM3C0));
        break;

    case TIMER4A:
		pTCCRX = &TCCR4B;
        if( wmode & 0x10 )
            TCCR4A = (TCCR4A | _BV(COM4A0));
        else
            TCCR4A = (TCCR4A & ~_BV(COM4A0));
        break;
    case TIMER4B:
		pTCCRX = &TCCR4B;
        if( wmode & 0x10 )
            TCCR4A = (TCCR4A | _BV(COM4B0));
        else
            TCCR4A = (TCCR4A & ~_BV(COM4B0));
        break;
    case TIMER4C:
		pTCCRX = &TCCR4B;
        if( wmode & 0x10 )
            TCCR4A = (TCCR4A | _BV(COM4C0));
        else
            TCCR4A = (TCCR4A & ~_BV(COM4C0));
        break;

    case TIMER5A:
		pTCCRX = &TCCR5B;
        if( wmode & 0x10 )
            TCCR5A = (TCCR5A | _BV(COM5A0));
        else
            TCCR5A = (TCCR5A & ~_BV(COM5A0));
        break;
    case TIMER5B:
		pTCCRX = &TCCR5B;
        if( wmode & 0x10 )
            TCCR5A = (TCCR5A | _BV(COM5B0));
        else
            TCCR5A = (TCCR5A & ~_BV(COM5B0));
        break;
    case TIMER5C:
		pTCCRX = &TCCR5B;
        if( wmode & 0x10 )
            TCCR5A = (TCCR5A | _BV(COM5C0));
        else
            TCCR5A = (TCCR5A & ~_BV(COM5C0));
        break;
    }

	if(pTCCRX == 0) return;

	if((fmode & 0x7f) == PWM_FREQ_SLOW) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_SLOW;	// prescale = 1024 (slowest mode)
	} else if((fmode & 0x7f) == PWM_FREQ_FAST) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_FAST; // prescale = 1 (fastest mode)
	} else if ((fmode & 0x7f) == PWM_FREQ_NORMAL) {
		*pTCCRX = (*pTCCRX & 0xf8) | PWM_FREQ_NORMAL;	// prescale = 64 (default)
	}
}

// enhanced PWM settings
// Function:
//	- set PWM frequency (unit: Hz), return maximum duty cycle 
// Note: 
//	- only PWM Timer1/3/4/5 support frequency update
unsigned int pwmFrequency(unsigned char pin, unsigned long fhz)
{
	unsigned long icrx = 0;
	unsigned char csxs = 0;
	unsigned char boost = 0;
	volatile unsigned char *pICRX = 0;
    unsigned long maxFreq = F_CPU;

	unsigned char timer = digitalPinToTimer(pin);

    if(timer == TIMER0A || timer == TIMER0B || timer == TIMER2A || timer == TIMER2B )
        return 0x0UL;

    switch(timer)
    {
    case TIMER1A:
    case TIMER1B:
        sbi(TCCR1B, WGM13);
        sbi(TCCR1B, WGM12);
        sbi(TCCR1A, WGM11);
        cbi(TCCR1A, WGM10);
		pICRX = &ICR1L;
		csxs = TCCR1B & 0x7;
        break;

    case TIMER3A:
    case TIMER3B:
    case TIMER3C:
        sbi(TCCR3B, WGM33);
        sbi(TCCR3B, WGM32);
        sbi(TCCR3A, WGM31);
        cbi(TCCR3A, WGM30);
		pICRX = &ICR3L;
		csxs = TCCR3B & 0x7;
        break;

    case TIMER4A:
    case TIMER4B:
    case TIMER4C:
        sbi(TCCR4B, WGM43);
        sbi(TCCR4B, WGM42);
        sbi(TCCR4A, WGM41);
        cbi(TCCR4A, WGM40);
		pICRX = &ICR4L;
		csxs = TCCR4B & 0x7;
        break;

    case TIMER5A:
    case TIMER5B:
    case TIMER5C:
        sbi(TCCR5B, WGM53);
        sbi(TCCR5B, WGM52);
        sbi(TCCR5A, WGM51);
        cbi(TCCR5A, WGM50);
		pICRX = &ICR5L;
		csxs = TCCR5B & 0x7;
        break;
    }


	if(pICRX == 0) return 0xff;

    if(boost)
    {
        maxFreq <<= 2;
    }

    switch( csxs )
    {
    case 1:
        break;
    case 2:
        maxFreq >>= 3;
        break;
    case 3:
        maxFreq >>= 6;
        break;
    case 4:
        maxFreq >>= 8;
        break;
    case 5:
        maxFreq >>= 10;
        break;
    }

    icrx = (maxFreq / fhz);
    if(icrx >= 65535u)
    {
        icrx = 65535u;
    }
	
	pwmAtomicWriteWord(pICRX, (unsigned int)icrx);

	return icrx;
}

// Function:
//	- return frequency (in Hz) by give PWM resolution (bits width of duty)
// Note: 
//	- timer0/2 works in FPWM mode, pwm frequency is fixed by given mode
//	- timer1/3/4/5 works in PCPWM mode, means frequency reduced by a half
unsigned long pwmResolution(unsigned char pin, unsigned char resBits)
{
	unsigned char csxs = 0;
	unsigned char boost = 0;
	unsigned long freq = 0x0UL;
    unsigned long maxFreq = F_CPU;
    unsigned long bitMask;

	unsigned char timer = digitalPinToTimer(pin);

    if(timer == TIMER0A || timer == TIMER0B || timer == TIMER2A || timer == TIMER2B )
        return 0x0UL;

    switch(timer)
    {
    case TIMER1A:
    case TIMER1B:
		csxs = TCCR1B & 0x7;
        break;
    case TIMER3B:
    case TIMER3C:
    case TIMER3A:
		csxs = TCCR3B & 0x7;
        break;
    case TIMER4A:
    case TIMER4B:
    case TIMER4C:
		csxs = TCCR4B & 0x7;
        break;
    case TIMER5C:
    case TIMER5B:
    case TIMER5A:
		csxs = TCCR5B & 0x7;
        break;
    }
	
    if(boost)
    {
        maxFreq <<= 2;
    }

    switch( csxs )
    {
    case 1:
        break;
    case 2:
        maxFreq >>= 3;
        break;
    case 3:
        maxFreq >>= 6;
        break;
    case 4:
        maxFreq >>= 8;
        break;
    case 5:
        maxFreq >>= 10;
        break;
    }
    
    bitMask = 1;
    bitMask <<= resBits;
    freq = maxFreq / bitMask;

	// update pwm frequency
	pwmFrequency(pin, freq);

	return freq;
}


// Exntenced PWM output
// Note: you can keep on use analogWrite() for compatible purpose!
void pwmWrite(unsigned char pin, unsigned int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);
	if (val == 0)
	{
		digitalWrite(pin, LOW);
	}
	else if (val == 255)
	{
		digitalWrite(pin, HIGH);
	}
	else
	{
		switch(digitalPinToTimer(pin))
		{
			case TIMER0A:
				// connect pwm to pin on timer 0, channel A
				sbi(TCCR0A, COM0A1);
				OCR0A = val; // set pwm duty
				break;
			case TIMER0B:
				// connect pwm to pin on timer 0, channel B
				sbi(TCCR0A, COM0B1);
				OCR0B = val; // set pwm duty
				break;

			case TIMER1A:
				// connect pwm to pin on timer 1, channel A
				sbi(TCCR1A, COM1A1);
				OCR1A = val; // set pwm duty
				break;
			case TIMER1B:
				// connect pwm to pin on timer 1, channel B
				sbi(TCCR1A, COM1B1);
				OCR1B = val; // set pwm duty
				break;

			case TIMER1C:
				// connect pwm to pin on timer 1, channel B
				sbi(TCCR1A, COM1C1);
				OCR1C = val; // set pwm duty
				break;

			case TIMER2A:
				// connect pwm to pin on timer 2, channel A
				sbi(TCCR2A, COM2A1);
				OCR2A = val; // set pwm duty
				break;

			case TIMER2B:
				// connect pwm to pin on timer 2, channel B
				sbi(TCCR2A, COM2B1);
				OCR2B = val; // set pwm duty
				break;

			case TIMER3A:
				// connect pwm to pin on timer 3, channel A
				sbi(TCCR3A, COM3A1);
				OCR3A = val; // set pwm duty
				break;

			case TIMER3B:
				// connect pwm to pin on timer 3, channel B
				sbi(TCCR3A, COM3B1);
				OCR3B = val; // set pwm duty
				break;

			case TIMER3C:
				// connect pwm to pin on timer 3, channel C
				sbi(TCCR3A, COM3C1);
				OCR3C = val; // set pwm duty
				break;

			case TIMER4A:
				//connect pwm to pin on timer 4, channel A
				sbi(TCCR4A, COM4A1);
				#if defined(COM4A0)		// only used on 32U4
				cbi(TCCR4A, COM4A0);
				#endif
				OCR4A = val;	// set pwm duty
				break;
			
			case TIMER4B:
				// connect pwm to pin on timer 4, channel B
				sbi(TCCR4A, COM4B1);
				OCR4B = val; // set pwm duty
				break;

			case TIMER4C:
				// connect pwm to pin on timer 4, channel C
				sbi(TCCR4A, COM4C1);
				OCR4C = val; // set pwm duty
				break;

			case TIMER5A:
				// connect pwm to pin on timer 5, channel A
				sbi(TCCR5A, COM5A1);
				OCR5A = val; // set pwm duty
				break;

			case TIMER5B:
				// connect pwm to pin on timer 5, channel B
				sbi(TCCR5A, COM5B1);
				OCR5B = val; // set pwm duty
				break;

			case TIMER5C:
				// connect pwm to pin on timer 5, channel C
				sbi(TCCR5A, COM5C1);
				OCR5C = val; // set pwm duty
				break;

			case NOT_ON_TIMER:
			default:
				if (val < 128) {
					digitalWrite(pin, LOW);
				} else {
					digitalWrite(pin, HIGH);
				}
		}
	}
}

#endif


