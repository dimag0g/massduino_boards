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

#ifndef DAP_h
#define DAP_h

#include <inttypes.h>

#ifdef __MD328P__

#define DAP_GA_X1	0X00
#define DAP_GA_X8	0X01
#define DAP_GA_X16	0X02
#define DAP_GA_X32	0x03

#define DAP_DNS_APN0	0X00
#define DAP_DNS_APN1	0X01
#define DAP_DNS_APN2	0X02
#define DAP_DNS_APN3	0x03
#define DAP_DNS_APN4	0x04
#define DAP_DNS_ADC		0x05
#define DAP_DNS_AGND	0x06
#define DAP_DNS_OFF		0x07

#define DAP_DPS_ADC		0X00
#define DAP_DPS_APP0	0X01
#define DAP_DPS_APP1	0X02
#define DAP_DPS_AGND	0x03


class DAPClass
{
  public:
	DAPClass() {};
		
	uint8_t begin(uint8_t GA, uint8_t dns, uint8_t dps);
	void end();
};

extern DAPClass DAP;

#endif

#endif

