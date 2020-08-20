/*
  AC0.cpp - AC0 library
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "Arduino.h"
#include "AC1.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/

/******************************************************************************
 * User API
 ******************************************************************************/

#ifdef __MD328P__

ISR(_VECTOR(25))
{
	if(AC1.funcHandler)
	{
		AC1.funcHandler();
	}
}

uint8_t AC1Class::begin(uint8_t outEnable, uint8_t positivePin, uint8_t negativePin, void (*f)())
{
	uint8_t c1srVal = _BV(C1IE);
	uint8_t c1xrVal = 0;
	uint8_t adcsrbVal = ADCSRB;
	
	if(outEnable)
	{
		c1xrVal |= _BV(C1OE);
	}
	
	switch(positivePin)
	{
	case AC1_PSEL_AC1P:
		break;
	case AC1_PSEL_ACXP:
		c1xrVal |= _BV(C1PS0);
		DIDR0 |= _BV(PE1D);
		break;
	case AC1_PSEL_DA0:
		c1srVal |= _BV(C1BG);
		break;
	case AC1_PSEL_NONE:
	default:
		c1xrVal |= _BV(C1PS0);
		c1srVal |= _BV(C1BG);
		break;
	}
	
	adcsrbVal &= 0xCF;
	switch(negativePin)
	{
	case AC1_NSEL_ACXN:
		DIDR1 |= _BV(PD7D);
		break;
	case AC1_NSEL_AC1N:
		adcsrbVal |= _BV(ACME10);
		DIDR0 |= _BV(PE3D);
		break;
	case AC1_NSEL_ADC_1P5:
		adcsrbVal |= _BV(ACME11);
		break;
	case AC1_NSEL_DAP:
	default:
		adcsrbVal |= _BV(ACME10);
		adcsrbVal |= _BV(ACME11);
		break;
	}
	
	C1XR = c1xrVal;
	ADCSRB = adcsrbVal;
	C1SR = c1srVal;
	
	funcHandler = f;
}

void AC1Class::end()
{
	C1SR = _BV(C1D);
	DIDR0 &= ~(_BV(PE1D));
	DIDR0 &= ~(_BV(PE3D));
	DIDR1 &= ~(_BV(PD7D));
}

AC1Class AC1;

#endif
