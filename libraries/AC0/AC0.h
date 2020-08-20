/*
  EEPROM.h - EEPROM library
  Copyright (c) 2006 David A. Mellis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef AC0_h
#define AC0_h

#include <inttypes.h>

#ifdef __MD328P__

#define AC0_PSEL_AC0P	0X00
#define AC0_PSEL_ACXP	0X01
#define AC0_PSEL_DA0	0X02
#define AC0_PSEL_NONE	0x03

#define AC0_NSEL_ACXN	0X00
#define AC0_NSEL_ADC	0X01
#define AC0_NSEL_DAP	0X02
#define AC0_NSEL_NONE	0x03


class AC0Class
{
  public:
	void (*funcHandler)();
	
	AC0Class() {};
		
	uint8_t begin(uint8_t outEnable, uint8_t positivePin, uint8_t negativePin, void (*f)());
	void end();
		
	//extern void (*func)();
};

extern AC0Class AC0;

#endif

#endif

