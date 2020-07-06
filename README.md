# Pool-Matrix-Monitor
## In_preparation
[![Generic badge](https://img.shields.io/badge/Version-1.0-yellow.svg)](https://github.com/fandau1/Pool-Matrix-Monitor/edit/master/README_CZ.md)

**Simple arduino pool clock with sensors**

<img src="image/matrix-cycle.gif" height="300" />

Pool-Matrix-Monitor is pool clock with temperature, ph sensor and all this data send to ThingSpeak, or GET to server.

# Features
  * Matrix display in cykle(TIME,"Outsite",OUTSITE_TEMPERATURE,"Pool",POOL_TEMPERATURE)
  * Automatic detection connected sensors
  * Auto correct summer and standart time
  * NTP synchronize time
  * send data to database (MYSQL), ThingSpeak

# Hardware
**Components:**
  * Basic
    * Arduino nano
    * DS3231
    * MAX7219 Matrix LED Display Module 4-v-1
  * Optional   
    * W5500 mini Ethernet
    * DS18B20 *- temperature sensor*
    * PH sensor PH-4502C 
  
**Schematic**
  * Version v1.1H **Arduino nano**

|  Arduino  |   |  Modules  |
| :-------: |---|  :------: |
| RESET | -> | W5500(RST) |
| D13(SCK) | -> | W5500(SCLK), MATRIX(CLK) |
| D12(MISO) | -> | W5500(MISO) |
| D11(MOSI) | -> | W5500(MOSI), MATRIX(DIN) |
| D10(SS) | -> | W5500(SCS) |
| D8 | -> | W5500(SCS) |
| D10(SS) | -> | MATRIX(CS) |
| D5(PWM) | -> | DS18B20 |
| A5(SCL) | -> | DS3231(SCL) |
| A4(SDA) | -> | DS3231(SDA) |
| A1 | -> | PH sensor |
| A0 | -> | Photo resistor(LUX meter) |

<img src="image/pool-scheme.png" height="600" />

# Installation
 * Download below library and upload code with Arduino IDE to arduino nano. Code is saved in **src/...** you can choose more version(no ethernet, etc.).
 
 **PS: In this time isn'nt code uploaded :) . In early time will be upload.**
 
**Library**
  * [MD_MAX72xx](https://github.com/MajicDesigns/MD_MAX72XX) - *from MajicDesigns*
  * [RTClib](https://github.com/adafruit/RTClib) - *from adafruit*
  * [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) - *from milesburton*
  * [Ethernet2](https://github.com/adafruit/Ethernet2) - *from adafruit*
  * [DST_RTC](https://github.com/andydoro/DST_RTC) - *from andydoro*
