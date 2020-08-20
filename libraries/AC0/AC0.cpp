/*
  AC0.cpp - AC0 library
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "Arduino.h"
#include "AC0.h"

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

ISR(_VECTOR(23))
{
	if(AC0.funcHandler)
	{
		AC0.funcHandler();
	}
}

uint8_t AC0Class::begin(uint8_t outEnable, uint8_t positivePin, uint8_t negativePin, void (*f)())
{
	uint8_t c0srVal = _BV(C0IE);
	uint8_t c0xrVal = 0;
	uint8_t adcsrbVal = ADCSRB;
	
	if(outEnable)
	{
		c0xrVal |= _BV(C0OE);
	}
	
	switch(positivePin)
	{
	case AC0_PSEL_AC0P:
		break;
	case AC0_PSEL_ACXP:
		c0xrVal |= _BV(C0PS0);
		break;
	case AC0_PSEL_DA0:
		c0srVal |= _BV(C0BG);
		break;
	case AC0_PSEL_NONE:
	default:
		c0xrVal |= _BV(C0PS0);
		c0srVal |= _BV(C0BG);
		break;
	}
	
	adcsrbVal &= 0x3F;
	switch(negativePin)
	{
	case AC0_NSEL_ACXN:
		break;
	case AC0_NSEL_ADC:
		adcsrbVal |= _BV(ACME00);
		break;
	case AC0_NSEL_DAP:
		adcsrbVal |= _BV(ACME01);
		break;
	case AC0_NSEL_NONE:
		default:
		adcsrbVal |= _BV(ACME00);
		adcsrbVal |= _BV(ACME01);
		break;
	}
	
	C0XR = c0xrVal;
	C0SR = c0srVal;
	ADCSRB = adcsrbVal;
	
	funcHandler = f;
}

void AC0Class::end()
{
	C0SR = _BV(C0D);
}

AC0Class AC0;

#endif
