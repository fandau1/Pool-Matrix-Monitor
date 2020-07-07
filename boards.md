# Boards Versions
|  Board Versions  | MCU | transfer type pool sensor | SW release | HW release |  Approximate price  |
| :--------: |:---:| :-----------------------: | :--------: | :--------: | :-----------------: |
| PoolMatrixNano | Arduino nano | cable | Not realesed | Not realesed | ~13$ |
| PoolMatrixNanoEth | Arduino nano | cable | Not realesed | Not realesed |  |
| PoolMatrixESP32 | ESP32 | cable OR WIFI | Not realesed | Not realesed |  |
| PoolMatrixESP8266 | ESP8266 | cable OR WIFI | Not realesed | Not realesed | |

# PoolMatrixNano
## Components
**Basic**
  * Arduino nano
  * DS3231
  * MAX7219 Matrix LED Display Module 4-v-1
  * some resistors
  
**Optional**
  * DS18B20 *- temperature sensor*
  * Photo Resistor
  * PH sensor PH-4502C 
    
## Schematic
|  Arduino  |   |  Modules  |
| :-------: |---|  :------: |
| D13(SCK) | -> |  MATRIX(CLK) |
| D11(MOSI) | -> |  MATRIX(DIN) |
| D10(SS) | -> | MATRIX(CS) |
| D5(PWM) | -> | DS18B20 |
| A5(SCL) | -> | DS3231(SCL) |
| A4(SDA) | -> | DS3231(SDA) |
| A1 | -> | PH sensor |
| A0 | -> | Photo resistor(LUX meter) |

<img src="image/pool-scheme.png" height="600" />
