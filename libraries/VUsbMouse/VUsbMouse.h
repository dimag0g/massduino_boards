/* Name: vmouse.h
 */

#ifndef __vmouse_h_included__
#define __vmouse_h_included__

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "usbdrv.h"

#ifdef __cplusplus
}
#endif

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */
/* This is the same report descriptor as seen in a Logitech mouse. The data
 * described by this descriptor consists of 4 bytes:
 *      .  .  .  .  . B2 B1 B0 .... one byte with mouse button states
 *     X7 X6 X5 X4 X3 X2 X1 X0 .... 8 bit signed relative coordinate x
 *     Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0 .... 8 bit signed relative coordinate y
 *     W7 W6 W5 W4 W3 W2 W1 W0 .... 8 bit signed relative coordinate wheel
 */
typedef struct {
	uchar	buttonMask;
	char	dx;
	char	dy;
	char 	dWheel;
} reportBuffer_t;


class VUsbMouseClass {
public:
	void begin();

	void loop();

public:
	uchar idleRate;
	reportBuffer_t reportBuffer;
};

extern VUsbMouseClass  VUsbMouse;
/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8]);


#endif /* __vmouse_h_included__ */
