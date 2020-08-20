/*
This example should run on most AVRs with only little changes. No special
hardware resources except INT0 are used. You may have to change usbconfig.h for
different I/O pins for USB. Please note that USB D+ must be the INT0 pin, or
at least be connected to INT0 as well.
*/
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "usbdrv.h"

#ifdef __cplusplus
}
#endif

#include "VUsbDataInOut.h"

static uchar    idleRate;           // in 4 ms units 
VUsbDataDevice VUsbData;

PROGMEM const char usbDescriptorConfiguration[] = {    /* USB configuration descriptor */
    9,          /* sizeof(usbDescriptorConfiguration): length of descriptor in bytes */
    USBDESCR_CONFIG,    /* descriptor type */
    18 + 7 * USB_CFG_HAVE_INTRIN_ENDPOINT + 7 * 1 +
                (USB_CFG_DESCR_PROPS_HID & 0xff), 0,
                /* total length of data returned (including inlined descriptors) */
    1,          /* number of interfaces in this configuration */
    1,          /* index of this configuration */
    0,          /* configuration name string index */
#if USB_CFG_IS_SELF_POWERED
    (1 << 7) | USBATTR_SELFPOWER,       /* attributes */
#else
    (1 << 7),                           /* attributes */
#endif
    USB_CFG_MAX_BUS_POWER/2,            /* max USB current in 2mA units */
/* interface descriptor follows inline: */
    9,          /* sizeof(usbDescrInterface): length of descriptor in bytes */
    USBDESCR_INTERFACE, /* descriptor type */
    0,          /* index of this interface */
    0,          /* alternate setting for this interface */
    USB_CFG_HAVE_INTRIN_ENDPOINT + 1, // + USB_CFG_HAVE_INTRIN_ENDPOINT3, /* endpoints excl 0: number of endpoint descriptors to follow */
    USB_CFG_INTERFACE_CLASS,
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    0,          /* string index for interface */
#if (USB_CFG_DESCR_PROPS_HID & 0xff)    /* HID descriptor */
    9,          /* sizeof(usbDescrHID): length of descriptor in bytes */
    USBDESCR_HID,   /* descriptor type: HID */
    0x01, 0x01, /* BCD representation of HID version */
    0x00,       /* target country code */
    0x01,       /* number of HID Report (or other HID class) Descriptor infos to follow */
    0x22,       /* descriptor type: report */
    USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH, 0,  /* total length of report descriptor */
#endif
#if USB_CFG_HAVE_INTRIN_ENDPOINT    /* endpoint descriptor for endpoint 1 */
    7,          /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,  /* descriptor type = endpoint */
    (char)0x81, /* IN endpoint number 1 */
    0x03,       /* attrib: Interrupt endpoint */
    8, 0,       /* maximum packet size */
    USB_CFG_INTR_POLL_INTERVAL, /* in ms */
#endif
#if USB_CFG_HAVE_INTRIN_ENDPOINT   /* endpoint descriptor for endpoint 1 */
    7,          /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,  /* descriptor type = endpoint */
    (char)(0x00 | 1), /* OUT endpoint number 1 */
    0x03,       /* attrib: Interrupt endpoint */
    8, 0,       /* maximum packet size */
    USB_CFG_INTR_POLL_INTERVAL, /* in ms */
#endif
};

PROGMEM const char usbHidReportDescriptor[27] = { /* USB report descriptor */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x00,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, REPORT_SIZE,           //   REPORT_COUNT (2)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};

int usbDescriptorStringSerialNumber[] = {
    USB_STRING_DESCRIPTOR_HEADER(8),
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

char Hex2Char(uint8_t hex)
{
    hex &= 0x0f;
    if(hex >= 0x0A)
    {
        hex -= 0x0A;
        hex += 'A';
    }
    else
    {
        hex += '0';
    }
    return hex;
}
void GetSerialNumber(void)
{
    uint8_t offset = 1;
    uint8_t i;
    uint32_t chipUid = readChipUid();
    uint8_t uidBytes[4];
    memcpy( (uint8_t *)uidBytes, (uint8_t *)&chipUid, 4 );
    for( i = 0; i < 4; i++ )
    {
        usbDescriptorStringSerialNumber[offset++] = Hex2Char(uidBytes[i] >> 4);
        usbDescriptorStringSerialNumber[offset++] = Hex2Char(uidBytes[i] & 0x0f);
    }
}

void VUsbDataDevice::begin (USB_DATA_OUT_CALLBACK dataCallback) {
	uchar i;

    GetSerialNumber();
    
    mDataCallback = dataCallback;

	TIMSK0 = 0;
    
	usbInit();
      
	usbDeviceDisconnect();
		
    	// fake USB disconnect for > 250ms
    	i = 0;
    	while(--i) {  
		nop(); nop();
		delayMicroseconds(1000);
	}

    usbDeviceConnect();

    // TODO: Remove the next two lines once we fix
    //       missing first keystroke bug properly.
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
    
void VUsbDataDevice::loop() {
    usbPoll();
  }


void VUsbDataDevice::sendData(uint8_t *buffer, uint8_t length) {
    uint8_t offset = 0;
    uint8_t sendLength;
    do
    {
        sendLength = length;
        if(sendLength > REPORT_SIZE)
        {
            sendLength = REPORT_SIZE;
        }
        if(sendLength > 0)
        {
            sendData_OnePacket(buffer + offset, sendLength);
            offset += sendLength;
            length -= sendLength;
        }
    }while(length > 0);
}

void VUsbDataDevice::sendData_OnePacket(uint8_t *buffer, uint8_t length) {
   if(length > REPORT_SIZE)
   {
       length = REPORT_SIZE;
   }

    while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keystroke
      //       so we know the previous keystroke was
      //       sent.
    }
      
    memset(reportBuffer, 0, sizeof(reportBuffer));
    
    memcpy(reportBuffer + 1, buffer, length);
    reportBuffer[0] = 1;
        
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

//  while (!usbInterruptIsReady()) {
//    // Note: We wait until we can send keystroke
//    //       so we know the previous keystroke was
//    //       sent.
//  }
//
//  // This stops endlessly repeating keystrokes:
//  memset(reportBuffer, 0, sizeof(reportBuffer));
//  usbSetInterrupt(reportBuffer, sizeof(reportBuffer));

}

void usbFunctionWriteOut(uchar *data, uchar len)
{
    if(VUsbData.mDataCallback)
    {
        if(len > 1)
        {
            len -= 1;
            (*VUsbData.mDataCallback)(data + 1, len);
        }
    }
}


//--------------------------------------------------------------------------------
// USB_PUBLIC uchar usbFunctionSetup
//--------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C"{
#endif 

usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq)
{
    if( rq->wValue.bytes[1] == USBDESCR_STRING )
    {
        if( rq->wValue.bytes[0] == 3 )  //USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER
        {
            usbMsgPtr = (usbMsgPtr_t)usbDescriptorStringSerialNumber;
            return (sizeof(usbDescriptorStringSerialNumber));
        }
    }

    return 0;
}

uchar usbFunctionSetup(uchar data[8]) 
  {
    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = VUsbData.reportBuffer; //
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
      /* class request type */

      if(rq->bRequest == USBRQ_HID_GET_REPORT){
	/* wValue: ReportType (highbyte), ReportID (lowbyte) */
	/* we only have one report type, so don't look at wValue */
        // TODO: Ensure it's okay not to return anything here?    
	return 0;

      }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
	//            usbMsgPtr = &idleRate;
	//            return 1;
	return 0;
      }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
	idleRate = rq->wValue.bytes[1];
      }
    }else{
      /* no vendor specific requests implemented */
    }
    return 0;
  }
#ifdef __cplusplus
} // extern "C"
#endif


/* ------------------------------------------------------------------------- */
