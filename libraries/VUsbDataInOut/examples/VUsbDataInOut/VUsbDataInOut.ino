#include <WDT.h>

// ==========================================================================
// MASSDUINO VUSB (HID Device) Demostration
//
// ==========================================================================
//                                                           VCC
// Usage Note:                                               ___
//      1. VUSB is type of low speed device                   |
//      2. Massduino VUSB apply to MD328P device only        |
//      3. VUSB IO connections:                              | |  1.5Kohm
//                                                            |
//              Massduino D4 <--------|| 50ohm ||--------------+---------> DP
//              Massduino D2 <--------|| 50ohm ||------------------------> DM
//
//	4. A 50ohm resister should be serial connected to DP/DM and D4/2 for isolation.
//	5. A 1.5Kohm pullup on D4 should be used to apply USB low speed identification.
//	6. IO used for DP/DM can be modified inside ../usbconfig.h, But note interrupter
//         on DP is needed to capture start of bus transaction.
//	7. VCC should be working at 3.3V (3.0V ~ 3.6V is accepted by USB spec),
//	   or clamp diodbe should be used to limit voltage of DP/DM.
//	8. NOTE: TMR0_OVF inerrupter is disabled inside of VUSB intiliazaiton,
//	   so arduino's delay() which based on TMR0_OVF interupter can not be used anymore.
//	   use arduino's delayMicroseconds() which is instruction based delay.

#include <VUsbDataInOut.h>

#define BUTTON_PIN 29

#define SEND_BUFF_SIZE  10
uint8_t sendBuffer[SEND_BUFF_SIZE];

// If the timer isr is corrected
// to not take so long change this to 0.
#define BYPASS_TIMER_ISR 1

void delay_ms(uint16_t ms) {
#if BYPASS_TIMER_ISR
  for (uint16_t i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
#else
  delay(ms);
#endif
}

void setup()
{
  noInterrupts();
  sysClock(EXT_OSC);

  wdt_reset();
  wdt_enable(WTO_256MS);

  pinMode(13, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);

  Serial.begin(115200);
  VUsbData.begin(usbOutDataCallback);
  interrupts();
  
  Serial.println("init over");
}

void usbOutDataCallback(uchar *data, uchar len)
{
  FlipLed();
  Serial.write(data, len);
}

void loop()
{
  wdt_reset();
  
  // VUSB event loop
  VUsbData.loop();

  if(digitalRead(BUTTON_PIN) == 0)
  {
    FlipLed();
    for(uint8_t i = 0; i < SEND_BUFF_SIZE; i++)
    {
      sendBuffer[i] = i + 48;
    }
    VUsbData.sendData((uint8_t *)sendBuffer, sizeof(sendBuffer));

    delay_ms(20);
  }
}

bool g_Flip;
void FlipLed()
{
  digitalWrite(13, g_Flip);
  g_Flip = !g_Flip;
}


