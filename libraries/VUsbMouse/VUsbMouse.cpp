/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/
#include "Arduino.h"
#include "VUsbMouse.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "usbdrv.h"

#ifdef __cplusplus
}
#endif

VUsbMouseClass  VUsbMouse;

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM const char usbHidReportDescriptor[52] = { /* USB report descriptor, size must match usbconfig.h */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xA1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM
    0x29, 0x03,                    //     USAGE_MAXIMUM
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Const,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xC0,                          //   END_COLLECTION
    0xC0,                          // END COLLECTION
};

void VUsbMouseClass::begin() {
	uchar   i;

	TIMSK0 = 0;

	memset(&reportBuffer, 0, sizeof(reportBuffer));

	usbInit();
	usbDeviceDisconnect();

// fake USB disconnect for > 250ms
	i = 0;
	while(--i) {  
	nop(); nop();
		delayMicroseconds(1000);
	}

	usbDeviceConnect();
}

void VUsbMouseClass::loop() {
		usbPoll();
		if(usbInterruptIsReady()){
			/* called after every poll of the interrupt endpoint */
			usbSetInterrupt((uchar *)&reportBuffer, sizeof(reportBuffer));
	}
}

#ifdef __cplusplus
extern "C" {
#endif

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
  usbRequest_t    *rq = (usbRequest_t *)data;

    /* The following requests are never used. But since they are required by
     * the specification, we implement them in this example.
     */
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            usbMsgPtr = (uchar *)&VUsbMouse.reportBuffer;
            return sizeof(reportBuffer_t);
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = (uchar *)&VUsbMouse.idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            VUsbMouse.idleRate = rq->wValue.bytes[1];
        }
    } else {
        /* no vendor specific requests implemented */
    }
    return 0;   /* default for not implemented requests: return no data back to host */
}

#ifdef __cplusplus
}
#endif


/* ------------------------------------------------------------------------- */
