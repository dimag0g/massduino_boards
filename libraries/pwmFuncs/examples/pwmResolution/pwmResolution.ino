#include <pwmFuncs.h>

/*
 * The pwm bits and frequence table:
bits:1, freq:8000000, maxDuty:2
bits:2, freq:4000000, maxDuty:4
bits:3, freq:2000000, maxDuty:8
bits:4, freq:1000000, maxDuty:16
bits:5, freq:500000, maxDuty:32
bits:6, freq:250000, maxDuty:64
bits:7, freq:125000, maxDuty:128
bits:8, freq:62500, maxDuty:256
bits:9, freq:31250, maxDuty:512
bits:10, freq:15625, maxDuty:1024
bits:11, freq:7812, maxDuty:2048
bits:12, freq:3906, maxDuty:4096
bits:13, freq:1953, maxDuty:8192
bits:14, freq:976, maxDuty:16393
bits:15, freq:488, maxDuty:32786
bits:16, freq:244, maxDuty:65535
 */

uint32_t bits;
uint32_t freq;
uint32_t maxDuty;

#define PWM_PIN     13

void setup() {
  // put your setup code here, to run once:
  pwmMode(PWM_PIN, PWM_MODE_NORMAL, PWM_FREQ_FAST, 0);
  pwmResolution(PWM_PIN, 16);
  pwmWrite(PWM_PIN, 200 - 1);
  
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
//  for(bits = 1; bits <= 16; bits ++)
//  {
//    freq = pwmResolution(PWM_PIN, bits);
//    maxDuty = pwmFrequency(PWM_PIN, freq);
//    Serial.print("bits:");
//    Serial.print(bits);
//    Serial.print(", freq:");
//    Serial.print(freq);
//    Serial.print(", maxDuty:");
//    Serial.println(maxDuty);
//  }

  while(1);
}
