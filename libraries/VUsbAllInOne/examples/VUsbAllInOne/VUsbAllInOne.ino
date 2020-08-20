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

#include <VUsbAllInOne.h>

#define BUTTON_PIN 29
#define BUTTON_PIN_KEY 30
#define BUTTON_PIN_MOUSE 32

#define SEND_BUFF_SIZE  10
uint8_t sendBuffer[SEND_BUFF_SIZE];

// to generate coordinate for circle-rounded mouse
int sinus = 7 << 6;
int cosinus = 0;
void advanceCircleByFixedAngle();


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
  pinMode(BUTTON_PIN_KEY, INPUT);
  digitalWrite(BUTTON_PIN_KEY, HIGH);
  pinMode(BUTTON_PIN_MOUSE, INPUT);
  digitalWrite(BUTTON_PIN_MOUSE, HIGH);
  
  Serial.begin(115200);
  VUsbAll.begin(usbOutDataCallback);
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
  VUsbAll.loop();

  if(digitalRead(BUTTON_PIN) == 0)
  {
    FlipLed();
    Report_Data();
    delay_ms(20);
  }

  if(digitalRead(BUTTON_PIN_KEY) == 0)
  {
    FlipLed();
    Report_Key();
    delay_ms(20);
  }

  if(digitalRead(BUTTON_PIN_MOUSE) == 0)
  {
    FlipLed();
    Report_Mouse();
    delay_ms(20);
  }
}

bool g_Flip;
void FlipLed()
{
  digitalWrite(13, g_Flip);
  g_Flip = !g_Flip;
}

void Report_Data()
{
    for (uint8_t i = 0; i < SEND_BUFF_SIZE; i++)
    {
      sendBuffer[i] = 48 + i;
    }
    VUsbAll.sendData((uint8_t *)sendBuffer, sizeof(sendBuffer));
}

void Report_Key()
{
  VUsbAll.sendKeyStroke(KEY_H);
  VUsbAll.sendKeyStroke(KEY_E);
  VUsbAll.sendKeyStroke(KEY_L);
  VUsbAll.sendKeyStroke(KEY_L);
  VUsbAll.sendKeyStroke(KEY_O);

  VUsbAll.sendKeyStroke(KEY_SPACE);

  VUsbAll.sendKeyStroke(KEY_W);
  VUsbAll.sendKeyStroke(KEY_O);
  VUsbAll.sendKeyStroke(KEY_R);
  VUsbAll.sendKeyStroke(KEY_L);
  VUsbAll.sendKeyStroke(KEY_D);

  VUsbAll.sendKeyStroke(KEY_ENTER);
}

void Report_Mouse()
{
  advanceCircleByFixedAngle();
}

/* The following function advances sin/cos by a fixed angle
   and stores the difference to the previous coordinates in the report
   descriptor.
   The algorithm is the simulation of a second order differential equation.
*/
#define DIVIDE_BY_64(val)  ((val + (val > 0 ? 32 : -32)) >> 7)    /* rounding divide */

void advanceCircleByFixedAngle()
{
  char    d1, d2;

  d1 = DIVIDE_BY_64(cosinus);
  d2 = DIVIDE_BY_64(sinus);
  
  sinus += d1;
  VUsbAll.sendMouseReport(0, d1, d2, 0);
  cosinus -= d2;
}



