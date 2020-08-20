/*
  AC1.h - AC1 library
*/

#ifndef AC1_h
#define AC1_h

#include <inttypes.h>

#ifdef __MD328P__

#define AC1_PSEL_AC1P	0X00
#define AC1_PSEL_ACXP	0X01
#define AC1_PSEL_DA0	0X02
#define AC1_PSEL_NONE	0x03

#define AC1_NSEL_ACXN	0X00
#define AC1_NSEL_AC1N	0X01
#define AC1_NSEL_ADC_1P5	0X02
#define AC1_NSEL_DAP	0x03


class AC1Class
{
  public:
	void (*funcHandler)();
	
	AC1Class() {};
		
	uint8_t begin(uint8_t outEnable, uint8_t positivePin, uint8_t negativePin, void (*f)());
	void end();
		
	//extern void (*func)();
};

extern AC1Class AC1;

#endif

#endif

