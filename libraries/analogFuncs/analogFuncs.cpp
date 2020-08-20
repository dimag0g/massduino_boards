/*
  analogFuncs.h - Add more analog functions support for 11 to 16 bits resolution
  
  History:
	August 04, 2018 by Tony Tan
	
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
#include <analogFuncs.h>
#include "wiring_private.h"
#include "pins_arduino.h"


#if defined(__MD328D__) || defined(__MD328P__)
extern unsigned char analog_reference;
extern void analogReference(unsigned char mode);
extern int __analogRead(unsigned char pin);
int analogRead_Bsp(unsigned int sampleCount, unsigned int divCount);

int analogRead_Bsp(unsigned int sampleCount, unsigned int divCount)
{
    unsigned char low, high;
    unsigned int i, j;
    unsigned char validCnt;
    unsigned int advTmp;
    unsigned long advSum = 0;
    //unsigned int adcBuffer[buffSize];

    //delay(1);
    for( i = 0; i < sampleCount; i++ )
    {
        // start the conversion
        sbi(ADCSRA, ADSC);

        // ADSC is cleared when the conversion finishes
        while (bit_is_set(ADCSRA, ADSC));

        // we have to read ADCL first; doing so locks both ADCL
        // and ADCH until ADCH is read.  reading ADCL second would
        // cause the results of each conversion to be discarded,
        // as ADCL and ADCH would be locked when it completed.
        low  = ADCL;
        high = ADCH;

        // combine the two bytes
        //advTmp = (high << 6) | (low >> 2);
        advTmp = high;
        advTmp <<= 8;
        advTmp |= low;

//  	if(analog_reference == DEFAULT)
//  		advTmp *= 0.98;
//  	else if(analog_reference == EXTERNAL)
//  		advTmp *= 0.97;

        //adcBuffer[i] = advTmp;

        advSum += advTmp;
    }

    advTmp = advSum / divCount;

    return (int)advTmp;
}

int analogRead_10bits(unsigned char pin)
{
	unsigned int pVal;
#if defined(__MD328P__)
	unsigned int nVal;
	
	// enable/disable internal 1/5VCC channel
	ADCSRD &= 0xf0;
    if(pin == V5D1 || pin == V5D4) {
        ADCSRD |= 0x06;
    }
#endif

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
	if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
	if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
	pin = analogPinToChannel(pin);
#else
	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
	pin = analogPinToChannel(pin);
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif
  
	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
	ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);
#if defined(__MD328D__) || defined(__MD328P__)
    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(16, 64);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    
    return advTmp;
    //return (advTmp >> 2);
#else
#if defined(ADCSRA) && defined(ADCL)
	#if defined(__MD328P__)
	sbi(ADCSRC, SPN);
	nVal = adcRead();
	cbi(ADCSRC, SPN);
	#endif
	
	pVal = adcRead();

	#if defined(__MD328P__)
	pVal = (pVal + nVal) >> 1;
	#endif
#else
	// we dont have an ADC, return 0
	pVal = 0;
#endif

	// gain-error correction
#if defined(__MD328D__)
	pVal -= (pVal >> 5);
#elif defined(__MD328P__)
	pVal -= (pVal >> 7);
#endif
	// standard device from atmel
	return pVal;
#endif
}

int analogRead_11bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(16, 32);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    return advTmp;
}

int analogRead_12bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(25, 25);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    return advTmp;
}

int analogRead_13bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(32, 16);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    return advTmp;
}

int analogRead_14bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(64, 16);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    return advTmp;
}

int analogRead_15bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    unsigned int advTmp = analogRead_Bsp(128, 16);
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    return advTmp;
}

unsigned int analogRead_16bits(unsigned char pin)
{
    unsigned char low, high;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
    if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
    if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#elif defined(analogPinToChannel) && (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
    pin = analogPinToChannel(pin);
#else
    if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)
    pin = analogPinToChannel(pin);
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
    // the MUX5 bit of ADCSRB selects whether we're reading from channels
    // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

    // set the analog reference (high two bits of ADMUX) and select the
    // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
    // to 0 (the default).
#if defined(ADMUX)
    ADMUX = (analog_reference << 6) | (pin & 0x1f);
#endif

    unsigned int advTmp;
    unsigned int adcFilterCount;
    unsigned long adcFilterSum;

    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel

    //delay(1);
//  adcFilterCount = 0;
//  adcFilterSum = 0;
//  while( adcFilterCount < ANALOG_ADC_OVER_SAMPLE_COUNT )
//  {
//      //adcFilterSum += analogRead_Bsp(25, 16);
//      //adcFilterSum += analogRead_Bsp(16, 10);
//      //adcFilterSum += analogRead_Bsp(10, 5);
//      adcFilterSum += analogRead_Bsp(8, 4);
//      adcFilterCount++;
//  }
//
//  advTmp = adcFilterSum / ANALOG_ADC_OVER_SAMPLE_DIV;
//  if(analog_reference == DEFAULT)
//		advTmp *= 0.98;
//	else if(analog_reference == EXTERNAL)
//		advTmp *= 0.97;
    advTmp = analogRead_Bsp(256u, 16);
    return advTmp;
}


#elif defined(__AVR_ATmega2560__)

extern unsigned char analog_reference;

int analogRead_Average(unsigned int sampleCount, unsigned int divCount)
{
    unsigned char low, high;
    unsigned int i, j;
    unsigned char validCnt;
    unsigned int advTmp;
    unsigned long advSum = 0;

    //delay(1);
    for( i = 0; i < sampleCount; i++ )
    {
        // start the conversion
        sbi(ADCSRA, ADSC);

        // ADSC is cleared when the conversion finishes
        while (bit_is_set(ADCSRA, ADSC));

        // we have to read ADCL first; doing so locks both ADCL
        // and ADCH until ADCH is read.  reading ADCL second would
        // cause the results of each conversion to be discarded,
        // as ADCL and ADCH would be locked when it completed.
        low  = ADCL;
        high = ADCH;

        // combine the two bytes
        //advTmp = (high << 6) | (low >> 2);
        advTmp = high;
        advTmp <<= 8;
        advTmp |= low;

        advSum += advTmp;
    }

    advTmp = advSum / divCount;

    return (int)advTmp;
}


int analogRead_Bsp(unsigned char pin, unsigned int sampleCount, unsigned int divCount)
{
#if defined(__AVR_ATmega2560__)
	if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#else
	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif
  
	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
	#if defined(__MD328D__) || defined(__MD328P__)
	ADMUX = (analog_reference << 6) | (pin & 0x1f);
	#else
	ADMUX = (analog_reference << 6) | (pin & 0x07);
	#endif
#endif

	#if defined(__MD328D__) || defined(__MD328P__)
    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    //ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel
	#else
    //Set ADC clock
    ADCSRA &= 0xf8;
    //ADCSRA |= 0x00;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x01;         //Adc Clock = fosc / 2
    //ADCSRA |= 0x02;         //Adc Clock = fosc / 4
    //ADCSRA |= 0x03;         //Adc Clock = fosc / 8
    ADCSRA |= 0x04;         //Adc Clock = fosc / 16
    //ADCSRA |= 0x05;           //Adc Clock = fosc / 32
    //ADCSRA |= 0x06;           //Adc Clock = fosc / 64
    //ADCSRA |= 0x07;           //Adc Clock = fosc / 128
    // without a delay, we seem to read from the wrong channel
	#endif
	
	return analogRead_Average(sampleCount, divCount);
}

int analogRead_10bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 16, 16);
}

int analogRead_11bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 32, 16);
}

int analogRead_12bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 64, 16);
}

int analogRead_13bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 128, 16);
}

int analogRead_14bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 256, 16);
}

int analogRead_15bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 512u, 16);
}

unsigned int analogRead_16bits(unsigned char pin)
{
	return analogRead_Bsp(pin, 1024u, 16);
}
#endif
