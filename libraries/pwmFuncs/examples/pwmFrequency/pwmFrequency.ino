#include <pwmFuncs.h>

/*
 * the pwm freq and max duty table:
freq:100, maxDuty:65535
freq:200, maxDuty:65535
freq:300, maxDuty:53333
freq:400, maxDuty:40000
freq:500, maxDuty:32000
freq:600, maxDuty:26666
freq:700, maxDuty:22857
freq:800, maxDuty:20000
freq:900, maxDuty:17777
freq:1000, maxDuty:16000
freq:1100, maxDuty:14545
freq:1200, maxDuty:13333
freq:1300, maxDuty:12307
freq:1400, maxDuty:11428
freq:1500, maxDuty:10666
freq:1600, maxDuty:10000
freq:1700, maxDuty:9411
freq:1800, maxDuty:8888
freq:1900, maxDuty:8421
freq:2000, maxDuty:8000
freq:2100, maxDuty:7619
freq:2200, maxDuty:7272
freq:2300, maxDuty:6956
freq:2400, maxDuty:6666
freq:2500, maxDuty:6400
freq:2600, maxDuty:6153
freq:2700, maxDuty:5925
freq:2800, maxDuty:5714
freq:2900, maxDuty:5517
freq:3000, maxDuty:5333
freq:3100, maxDuty:5161
freq:3200, maxDuty:5000
freq:3300, maxDuty:4848
freq:3400, maxDuty:4705
freq:3500, maxDuty:4571
freq:3600, maxDuty:4444
freq:3700, maxDuty:4324
freq:3800, maxDuty:4210
freq:3900, maxDuty:4102
freq:4000, maxDuty:4000
freq:4100, maxDuty:3902
freq:4200, maxDuty:3809
freq:4300, maxDuty:3720
freq:4400, maxDuty:3636
freq:4500, maxDuty:3555
freq:4600, maxDuty:3478
freq:4700, maxDuty:3404
freq:4800, maxDuty:3333
freq:4900, maxDuty:3265
freq:5000, maxDuty:3200
freq:5100, maxDuty:3137
freq:5200, maxDuty:3076
freq:5300, maxDuty:3018
freq:5400, maxDuty:2962
freq:5500, maxDuty:2909
freq:5600, maxDuty:2857
freq:5700, maxDuty:2807
freq:5800, maxDuty:2758
freq:5900, maxDuty:2711
freq:6000, maxDuty:2666
freq:6100, maxDuty:2622
freq:6200, maxDuty:2580
freq:6300, maxDuty:2539
freq:6400, maxDuty:2500
freq:6500, maxDuty:2461
freq:6600, maxDuty:2424
freq:6700, maxDuty:2388
freq:6800, maxDuty:2352
freq:6900, maxDuty:2318
freq:7000, maxDuty:2285
freq:7100, maxDuty:2253
freq:7200, maxDuty:2222
freq:7300, maxDuty:2191
freq:7400, maxDuty:2162
freq:7500, maxDuty:2133
freq:7600, maxDuty:2105
freq:7700, maxDuty:2077
freq:7800, maxDuty:2051
freq:7900, maxDuty:2025
freq:8000, maxDuty:2000
freq:8100, maxDuty:1975
freq:8200, maxDuty:1951
freq:8300, maxDuty:1927
freq:8400, maxDuty:1904
freq:8500, maxDuty:1882
freq:8600, maxDuty:1860
freq:8700, maxDuty:1839
freq:8800, maxDuty:1818
freq:8900, maxDuty:1797
freq:9000, maxDuty:1777
freq:9100, maxDuty:1758
freq:9200, maxDuty:1739
freq:9300, maxDuty:1720
freq:9400, maxDuty:1702
freq:9500, maxDuty:1684
freq:9600, maxDuty:1666
freq:9700, maxDuty:1649
freq:9800, maxDuty:1632
freq:9900, maxDuty:1616
freq:10000, maxDuty:1600
freq:10100, maxDuty:1584
freq:10200, maxDuty:1568
freq:10300, maxDuty:1553
freq:10400, maxDuty:1538
freq:10500, maxDuty:1523
freq:10600, maxDuty:1509
freq:10700, maxDuty:1495
freq:10800, maxDuty:1481
freq:10900, maxDuty:1467
freq:11000, maxDuty:1454
freq:11100, maxDuty:1441
freq:11200, maxDuty:1428
freq:11300, maxDuty:1415
freq:11400, maxDuty:1403
freq:11500, maxDuty:1391
freq:11600, maxDuty:1379
freq:11700, maxDuty:1367
freq:11800, maxDuty:1355
freq:11900, maxDuty:1344
freq:12000, maxDuty:1333
freq:12100, maxDuty:1322
freq:12200, maxDuty:1311
freq:12300, maxDuty:1300
freq:12400, maxDuty:1290
freq:12500, maxDuty:1280
freq:12600, maxDuty:1269
freq:12700, maxDuty:1259
freq:12800, maxDuty:1250
freq:12900, maxDuty:1240
freq:13000, maxDuty:1230
freq:13100, maxDuty:1221
freq:13200, maxDuty:1212
freq:13300, maxDuty:1203
freq:13400, maxDuty:1194
freq:13500, maxDuty:1185
freq:13600, maxDuty:1176
freq:13700, maxDuty:1167
freq:13800, maxDuty:1159
freq:13900, maxDuty:1151
freq:14000, maxDuty:1142
freq:14100, maxDuty:1134
freq:14200, maxDuty:1126
freq:14300, maxDuty:1118
freq:14400, maxDuty:1111
freq:14500, maxDuty:1103
freq:14600, maxDuty:1095
freq:14700, maxDuty:1088
freq:14800, maxDuty:1081
freq:14900, maxDuty:1073
freq:15000, maxDuty:1066
freq:15100, maxDuty:1059
freq:15200, maxDuty:1052
freq:15300, maxDuty:1045
freq:15400, maxDuty:1038
freq:15500, maxDuty:1032
freq:15600, maxDuty:1025
freq:15700, maxDuty:1019
freq:15800, maxDuty:1012
freq:15900, maxDuty:1006
freq:16000, maxDuty:1000
freq:16100, maxDuty:993
freq:16200, maxDuty:987
freq:16300, maxDuty:981
freq:16400, maxDuty:975
freq:16500, maxDuty:969
freq:16600, maxDuty:963
freq:16700, maxDuty:958
freq:16800, maxDuty:952
freq:16900, maxDuty:946
freq:17000, maxDuty:941
freq:17100, maxDuty:935
freq:17200, maxDuty:930
freq:17300, maxDuty:924
freq:17400, maxDuty:919
freq:17500, maxDuty:914
freq:17600, maxDuty:909
freq:17700, maxDuty:903
freq:17800, maxDuty:898
freq:17900, maxDuty:893
freq:18000, maxDuty:888
freq:18100, maxDuty:883
freq:18200, maxDuty:879
freq:18300, maxDuty:874
freq:18400, maxDuty:869
freq:18500, maxDuty:864
freq:18600, maxDuty:860
freq:18700, maxDuty:855
freq:18800, maxDuty:851
freq:18900, maxDuty:846
freq:19000, maxDuty:842
freq:19100, maxDuty:837
freq:19200, maxDuty:833
freq:19300, maxDuty:829
freq:19400, maxDuty:824
freq:19500, maxDuty:820
freq:19600, maxDuty:816
freq:19700, maxDuty:812
freq:19800, maxDuty:808
freq:19900, maxDuty:804
 */

uint32_t freq;
uint32_t maxDuty;

#define PWM_PIN     46

void setup() {
  // put your setup code here, to run once:
  pwmMode(PWM_PIN, PWM_MODE_NORMAL, PWM_FREQ_FAST, 0);

  maxDuty = pwmFrequency(PWM_PIN, 19900);
  pwmWrite(PWM_PIN, maxDuty - 10);
  
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
//  for(freq = 100; freq < 20000; freq += 100)
//  {
//    maxDuty = pwmFrequency(PWM_PIN, freq);
//    Serial.print("freq:");
//    Serial.print(freq);
//    Serial.print(", maxDuty:");
//    Serial.println(maxDuty);
//  }

  while(1);
}
