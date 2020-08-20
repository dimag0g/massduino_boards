/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 */
#ifndef __UsbDataInOut_h__
#define __UsbDataInOut_h__

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "usbdrv.h"

#ifdef __cplusplus
}
#endif

#define BUFFER_SIZE 8 // Minimum of 2: 1 for modifiers + 1 for keystroke 
#define REPORT_SIZE     (BUFFER_SIZE - 1)
/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */

typedef void (*USB_DATA_OUT_CALLBACK)(uchar *data, uchar len);

class VUsbDataDevice {
 public:
  void begin (USB_DATA_OUT_CALLBACK dataCallback);
  void loop();

  void sendData_OnePacket(uint8_t *buffer, uint8_t length);
  void sendData(uint8_t *buffer, uint8_t length);

  //private: TODO: Make friend?
  uchar    reportBuffer[8];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]
  
  USB_DATA_OUT_CALLBACK mDataCallback;
};

extern VUsbDataDevice VUsbData;

uchar usbFunctionSetup(uchar data[8]);


#endif // __UsbKeyboard_h__
