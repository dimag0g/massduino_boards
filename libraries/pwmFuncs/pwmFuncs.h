#ifndef _pwmFuncs_h
#define _pwmFuncs_h


#define PWM_MODE_NORMAL   0x80
#define PWM_MODE_COMPM0   0x00
#define PWM_MODE_COMPM1   0x10
#define PWM_MODE_COMPM2   0x10
#define PWM_MODE_COMPM3   0x10

#define PWM_MODE_SOLO     0x80
#define PWM_MODE_DUO0     0x00
#define PWM_MODE_DUO1     0x10
#define PWM_MODE_DUO2     0x10
#define PWM_MODE_DUO3     0x10


#define PWM_FREQ_BOOST    0x80
#define PWM_FREQ_FAST     0x01
#define PWM_FREQ_NORMAL   0x03
#define PWM_FREQ_SLOW     0x05


void pwmMode(unsigned char pin, unsigned char wmode, unsigned char fmode, unsigned char dband);
unsigned int pwmFrequency(unsigned char pin, unsigned long fhz);
unsigned long pwmResolution(unsigned char pin, unsigned char resBits);
void pwmWrite(unsigned char pin, unsigned int val);


#endif

