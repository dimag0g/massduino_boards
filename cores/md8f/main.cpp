#include <stdint.h>
#include <avr/wdt.h>
#include <Arduino.h>
#include <wiring_private.h>

#if defined(__MD328D__) || defined(__MD328P__)
asm("	.section .vectors\n"
    "	.weak __vector_26\n"
    "	jmp  __vector_26\n"
    "	.weak __vector_27\n"
    "	jmp  __vector_27\n"
    "	.weak __vector_28\n"
    "	jmp  __vector_28\n"
    "	.weak __vector_29\n"
    "	jmp  __vector_29\n"
    "	.section .text\n"
);
#endif

uint8_t getBootResetFlag(void)
{
    return GPIOR2;
}

uint32_t readChipUid(void)
{
	uint32_t uid = GUID3;
	uid <<= 8;
	uid |= GUID2;
	uid <<= 8;
	uid |= GUID1;
	uid <<= 8;
	uid |= GUID0;	
	
	return uid;
}

#if defined(__MD328D__) || defined(__MD328P__)
void __patch_wdt(void) \
	     __attribute__((naked)) \
	     __attribute__((section(".init3")));
void __patch_wdt(void)
{
	MCUSR = 0;
	wdt_disable();
}
#endif

#if defined(__MD328P__)
void unlockWrite(volatile uint8_t *p, uint8_t val)
{
	uint8_t _o_sreg = SREG;
	volatile uint8_t *cp = p; 

	if(p == &PMX1)
		cp = &PMX0;
	cli();
	*cp = 0x80;
	*p = val;
	SREG = _o_sreg;
}
#endif

void atomicWriteWord(volatile uint8_t *p, uint16_t val)
{
	uint8_t _o_sreg = SREG;

	cli();
	*(p + 1) = (uint8_t)(val >> 8);
	nop(); nop(); nop();
	*p = (uint8_t)val;
	SREG = _o_sreg;
}

#if defined(__MD328D__) || defined(__MD328P__)
void sysClock(uint8_t mode)
{
	if(mode == EXT_OSC) {
		// set to right prescale
		CLKPR = 0x80;
		CLKPR = 0x01;	

		asm volatile ("nop");
		asm volatile ("nop");

		// enable external crystal
		PMCR = 0x80;
		PMCR = 0x97;
		
		// waiting for crystal stable
		for(GPIOR0 = 0xff; GPIOR0 > 0; --GPIOR0);
		for(GPIOR0 = 0xff; GPIOR0 > 0; --GPIOR0);

		// switch to external crystal
		PMCR = 0x80;
		PMCR = 0xb7;

		// waiting for crystal stable
		for(GPIOR0 = 0xff; GPIOR0 > 0; --GPIOR0);
		for(GPIOR0 = 0xff; GPIOR0 > 0; --GPIOR0);
		// set to right prescale
		CLKPR = 0x80;
		CLKPR = 0x00;	
	} else if(mode == INT_OSC) {
		// prescaler settings
		CLKPR = 0x80;
		CLKPR = 0x01;	

		// switch to internal crystal
		GPIOR0 = PMCR & 0x9f;
		PMCR = 0x80;
		PMCR = GPIOR0;

		// disable external crystal
		GPIOR0 = PMCR & 0xfb;
		PMCR = 0x80;
		PMCR = GPIOR0;
	}
}	

#endif

void md8fx8x_init()
{
#if defined(__MD328D__)
// store ivref calibration 
//  GPIOR1 = VCAL1;
//  GPIOR2 = VCAL2;

#if defined(__MD8F_SSOP20__)
	GPIOR0 = PMXCR | 0x07;
	PMXCR = 0x80;
	PMXCR = GPIOR0;
#endif

// enable 1KB E2PROM 
	ECCR = 0x80;
	ECCR = 0x40;

// clock source settings
	if((VDTCR & 0x0C) == 0x0C) {
		// switch to external crystal
		sysClock(EXT_OSC);
	} else {
		CLKPR = 0x80;
		CLKPR = 0x01;
	}
#else
	// enable 32KRC for WDT
	GPIOR0 = PMCR | 0x10;
	PMCR = 0x80;
	PMCR = GPIOR0;

	// clock scalar to 16MHz
	CLKPR = 0x80;
	CLKPR = 0x01;
#endif

    //Disable SWD          
    MCUSR |= 0X80;
    MCUSR |= 0X80;         
}

int main(void)
{

#if defined(__MD8F__)
	md8fx8x_init();
#endif	

	init();

#if defined(USBCON)
	USBDevice.attach();
#endif
	
	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
        
	return 0;
}
