/*
  MsTimer5 is a small and very easy to use library to interface Timer5 with
  humans. It's called MsTimer5 because it "hardcodes" a resolution of 100us
  millisecond on timer5
*/
#include <MsTimer5.h>

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

  MsTimer5.setMicros(100, flash); // 100us period
  MsTimer5.start();
}

void loop()
{
}
