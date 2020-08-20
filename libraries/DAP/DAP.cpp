/*
  AC0.cpp - AC0 library
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "Arduino.h"
#include "DAP.h"

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

uint8_t DAPClass::begin(uint8_t GA, uint8_t dns, uint8_t dps)
{
	uint8_t value = _BV(DAPEN);
	
	switch(GA)
	{
	case DAP_GA_X1:
		break;
	case DAP_GA_X8:
		value |= _BV(GA0);
		break;
	case DAP_GA_X16:
		value |= _BV(GA1);
		break;
	case DAP_GA_X32:
		value |= _BV(GA0);
		value |= _BV(GA1);
		break;
	}
	
	switch(dns)
	{
	case DAP_DNS_APN0:
		break;
	case DAP_DNS_APN1:
		value |= _BV(DNS0);
		break;
	case DAP_DNS_APN2:
		value |= _BV(DNS1);
		break;
	case DAP_DNS_APN3:
		value |= _BV(DNS1) | _BV(DNS0);
		break;
	case DAP_DNS_APN4:
		value |= _BV(DNS2);
		break;
	case DAP_DNS_ADC:
		value |= _BV(DNS2) | _BV(DNS0);
		break;
	case DAP_DNS_AGND:
		value |= _BV(DNS2) | _BV(DNS1);
		break;
	case DAP_DNS_OFF:
		value |= _BV(DNS2) | _BV(DNS1) | _BV(DNS0);
		break;
	}
	
	switch(dps)
	{
	case DAP_DPS_ADC:
		break;
	case DAP_DPS_APP0:
		value |= _BV(DPS0);
		break;
	case DAP_DPS_APP1:
		value |= _BV(DPS1);
		break;
	case DAP_DPS_AGND:
		value |= _BV(DPS1) | _BV(DPS0);
		break;
	}
	
	DAPCR = value;
}

void DAPClass::end()
{
	DAPCR = 0x00;
}

DAPClass DAP;

#endif
