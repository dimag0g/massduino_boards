#include <WDT.h>

void setup() {
  // put your setup code here, to run once:
  pinMode(0, OUTPUT);

  // delay to check wdt reset condition
  delay(1000);
#if defined(__MD328P__)
  wdt_enable(WTO_2S);
#else
  wdt_enable(WTO_512MS);
#endif

}

void loop() {
  // put your main code here, to run repeatedly:
  //wdt_reset();
  digitalToggle(0);
  delay(1);
}
