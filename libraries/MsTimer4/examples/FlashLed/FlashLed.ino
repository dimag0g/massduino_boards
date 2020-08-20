/*
  MsTimer4 is a small and very easy to use library to interface Timer4 with
  humans. It's called MsTimer4 because it "hardcodes" a resolution of 100us
  millisecond on timer4
*/
#include <MsTimer4.h>

// Switch on LED on and off each half second

#if ARDUINO >= 100
const int led_pin = LED_BUILTIN;	// 1.0 built in LED pin var
#else
const int led_pin = 13;			// default to pin 13
#endif


void flash()
{
  static boolean output = HIGH;
  
  digitalWrite(led_pin, output);
  output = !output;
}

void setup()
{
  pinMode(led_pin, OUTPUT);

  MsTimer4.set(500, flash); // 500ms period
  MsTimer4.start();
}

void loop()
{
}
