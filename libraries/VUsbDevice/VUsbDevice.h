/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 */
#ifndef __UsbDevice_h__
#define __UsbDevice_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbdrv.h"

#ifdef __cplusplus
}
#endif

class VUsbGenericDevice {
public:
  void begin();
    
  // TODO: Deprecate update
  void loop();
};

extern VUsbGenericDevice VUsbDevice;

#endif // __UsbDevice_h__
