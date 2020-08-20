/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 */
#ifndef __UsbAllInOne_h__
#define __UsbAllInOne_h__

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
/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_ENTER   40

#define KEY_SPACE   44

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

#define KEY_ARROW_LEFT 0x50


typedef void (*USB_DATA_OUT_CALLBACK)(uchar *data, uchar len);

/* This is the same report descriptor as seen in a Logitech mouse. The data
 * described by this descriptor consists of 4 bytes:
 *      .  .  .  .  . B2 B1 B0 .... one byte with mouse button states
 *     X7 X6 X5 X4 X3 X2 X1 X0 .... 8 bit signed relative coordinate x
 *     Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0 .... 8 bit signed relative coordinate y
 *     W7 W6 W5 W4 W3 W2 W1 W0 .... 8 bit signed relative coordinate wheel
 */
typedef struct {
    uchar   reportId;
	uchar	buttonMask;
	char	dx;
	char	dy;
	char 	dWheel;
} mouseReportBuffer_t;


class VUsbAllDevice {
 public:
  void begin (USB_DATA_OUT_CALLBACK dataCallback);
  void loop();

  void sendData_OnePacket(uint8_t *buffer, uint8_t length);
  void sendData(uint8_t *buffer, uint8_t length);

  void sendKeyStroke(uint8_t keyStroke);
  void sendKeyStroke(uint8_t keyStroke, uint8_t modifiers);

  void sendMouseReport(uint8_t button, uint8_t dx, uint8_t dy, uint8_t wheel);

  //private: TODO: Make friend?
  uchar idleRate;
  uchar reportBuffer[8];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]
  //mouseReportBuffer_t mouseReportBuffer;

  USB_DATA_OUT_CALLBACK mDataCallback;
};

extern VUsbAllDevice VUsbAll;

uchar usbFunctionSetup(uchar data[8]);


#endif // __UsbKeyboard_h__
