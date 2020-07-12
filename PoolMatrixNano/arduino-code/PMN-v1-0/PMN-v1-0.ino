/*
 * Version: v1.0

 */
/* Import library */
#include <RTClib.h>
#include <Wire.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <DST_RTC.h>
#include <avr/wdt.h>

/* Define pins you can change if you want */
#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN 8
#define PIN_SENSOR_DS18B20 5
#define PIN_SENSOR_PH A1 
#define PIN_SENSOR_LDR A0 

/* Import library */
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define SPEED_TIME  50
#define PAUSE_TIME  100
#define MAX_MESG  8

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

/* Configure RTC DS3231 */
//hodiny DS3231
RTC_DS3231 rtc; // SDA A4, SCL A5
DST_RTC dst_rtc; // DST object

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/* Configure DS18B20 */
OneWire oneWireDS1(PIN_SENSOR_DS18B20);
DallasTemperature sensorsDS1(&oneWireDS1);

/* Constatns for LDR sensor */
// These constants, define values needed for the LDR readings and ADC
#define MAX_ADC_READING           1023
#define ADC_REF_VOLTAGE           5.0
#define REF_RESISTANCE            4700 // measure this for best results
#define LUX_CALC_SCALAR           12518931
#define LUX_CALC_EXPONENT         -1.405

//LDR sensor variables
int sensorLdrCount;
float sensorLdrAvg; //Average
float sensorLdrSum;
int sensorLdrRawSum;
int sensorLdrRawAvg; //Average
unsigned long sensorLdrTime;
unsigned long sensorLdrPeriod = 1000; //Period for LDR measurement in 10 count does mean with 6000 = 6s*10 = 1 minute for LDR measurement

//DS18B20 variables
float sensor1T; //T as temperature
float sensor2T;

//variable for PH sensor
float sensorPh;

//variable for conntected sensors
bool sensorsConnect [3];

//variables for matrix display
char  szMesg[MAX_MESG+1] = "Boot";
int matrixState;
unsigned long matrixTime;
unsigned long matrixPeriod; 



void setup() {
  // initialization of communication via serial line at 9600 baud
  Serial.begin(9600);
  
  // Nastavení maticovky
  P.begin();
  P.setIntensity (3); //0-15 
  P.setZone(0, 0, MAX_DEVICES-1);
  P.setIntensity (3); //0-15 
  P.displayZoneText(0, szMesg, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  //P.displayZoneText(0, szMesg, SPEED_TIME, PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  //run rtc
  Wire.begin();
  rtc.begin();

  //Setup DS18B20
  sensorsDS1.begin();

  wdt_enable(WDTO_8S);
}


void loop() {
  Intensity();
  MatrixCycle();
  wdt_reset();
}

void MatrixCycle(){ 
  P.displayAnimate();
  if (P.getZoneStatus(0)){  
    if(matrixState == 0 && millis() - matrixTime >= matrixPeriod){
      DateTime standardTime = rtc.now();
      DateTime DstTime = dst_rtc.calculateTime(standardTime); // takes into account DST
      //move to 'szMesg' time in the form 'hh: mm' (that will appear on buffer and display on matrix)
      char buf1[] = "hh:mm";
      strcpy(szMesg, DstTime.toString(buf1));     
      SenzorsDS();
      sensorPH();
      matrixState++; 
      matrixPeriod = 4000; 
      matrixTime = millis();  
    }
    else if(matrixState == 1 && sensorsConnect [0] == 1 && millis() - matrixTime >= matrixPeriod){
      strcpy(szMesg, "Outside"); //You can change
      matrixState++; 
      matrixPeriod = 1500;   
      matrixTime = millis();   
    }
    else if(matrixState == 1 && sensorsConnect [0] == 0 && millis() - matrixTime >= matrixPeriod){
      matrixState = 3;   
    }
    else if(matrixState == 2 && millis() - matrixTime >= matrixPeriod){
      dtostrf(sensor1T, 3, 2, szMesg);
      matrixState++;             
      matrixPeriod = 4000;  
      matrixTime = millis();      
    }
    else if(matrixState == 3 && sensorsConnect [1] == 1 && millis() - matrixTime >= matrixPeriod){
      strcpy(szMesg, "Pool"); //You can change
      matrixState++; 
      matrixPeriod = 1500;
      matrixTime = millis();      
    }
    else if(matrixState == 3 && sensorsConnect [1] == 0 && millis() - matrixTime >= matrixPeriod){
      matrixState = 5;   
    }
    else if(matrixState == 4 && millis() - matrixTime >= matrixPeriod){
      dtostrf(sensor2T, 3, 2, szMesg);
      matrixState++; 
      matrixPeriod = 4000;
      matrixTime = millis();       
    }
    else if(matrixState == 5 && sensorsConnect [2] == 1 && millis() - matrixTime >= matrixPeriod){
      strcpy(szMesg, "PH sensor"); //You can change
      matrixState++; 
      matrixPeriod = 1500;
      matrixTime = millis();      
    }
    else if(matrixState == 5 && sensorsConnect [2] == 0 && millis() - matrixTime >= matrixPeriod){
      matrixState = 0;   
    }
    else if(matrixState == 6 && millis() - matrixTime >= matrixPeriod){
      dtostrf(sensorPh, 3, 2, szMesg);
      matrixState = 0; 
      matrixPeriod = 4000;
      matrixTime = millis();      
    }     
    P.displayReset(0);  
  } 
}

void SenzorsDS(){
  // Temperature reading
  sensorsDS1.requestTemperatures();
  sensor1T = sensorsDS1.getTempCByIndex(0); 
  sensor2T = sensorsDS1.getTempCByIndex(1);
  if(sensor1T == -127){
    sensorsConnect [0] = 0; 
  }
  else{
    sensorsConnect [0] = 1;   
  }
  if(sensor2T == -127){
    sensorsConnect [1] = 0;   
  }
  else{
    sensorsConnect [1] = 1;   
  }

}

void Intensity(){
  if(millis() - sensorLdrTime >= sensorLdrPeriod){
    // Perform the analog to digital conversion  
    int sensorLdrRaw = analogRead(0);
    /*// RESISTOR VOLTAGE_CONVERSION
    // Convert the raw digital data back to the voltage that was measured on the analog pin
    float sensorLdrResiV = (float)sensorLdrRaw / MAX_ADC_READING * ADC_REF_VOLTAGE;
    // voltage across the LDR is the 5V supply minus the 5k resistor voltage
    float sensorLdrV = ADC_REF_VOLTAGE - sensorLdrResiV;
    // LDR_RESISTANCE_CONVERSION
    // resistance that the LDR would have for that voltage  
    float sensorLdrR = sensorLdrV/sensorLdrResiV * REF_RESISTANCE;
    // LDR_LUX
    // Change the code below to the proper conversion from ldrResistance to
    // ldrLux
    float sensorLdrLux = LUX_CALC_SCALAR * pow(sensorLdrR, LUX_CALC_EXPONENT);*/
    float sensorLdrLux = LUX_CALC_SCALAR * pow(((ADC_REF_VOLTAGE - ((float)sensorLdrRaw / MAX_ADC_READING * ADC_REF_VOLTAGE))/((float)sensorLdrRaw / MAX_ADC_READING * ADC_REF_VOLTAGE) * REF_RESISTANCE), LUX_CALC_EXPONENT);
    sensorLdrSum = sensorLdrSum + sensorLdrLux;
    sensorLdrRawSum = sensorLdrRawSum + sensorLdrRaw;
    sensorLdrCount++;
    Serial.println(sensorLdrCount);
    Serial.print("RAW LDR: ");        
    Serial.println(sensorLdrRaw);    
    if(sensorLdrCount >= 10){
      sensorLdrAvg = sensorLdrSum / sensorLdrCount;
      sensorLdrRawAvg = sensorLdrRawSum / sensorLdrCount;      
      sensorLdrCount = 0;
      sensorLdrSum = 0;
      sensorLdrRawSum = 0;  
      sensorsConnect [3] = 1; 
      //setup automatic light of matrix
      if(sensorLdrRawAvg<=10){
        //<10 LDR is unconected
        P.setIntensity (3); // setup matrix intensity 0-15 
        sensorsConnect [3] = 0;    
      } 
      else if(sensorLdrRawAvg<=100){
        P.setIntensity (1); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg<=200){
        P.setIntensity (2); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg<=300){
        P.setIntensity (2); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg<=600){
        P.setIntensity (3); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg<=700){
        P.setIntensity (4); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg<=800){
        P.setIntensity (5); // setup matrix intensity 0-15 
      }
      else if(sensorLdrRawAvg>=900){
        P.setIntensity (8); // setup matrix intensity 0-15 
      }   
    }   
    sensorLdrTime = millis();
  }  
}

void sensorPH(){
  // create help variables
  int sensorPhArray[10];
  int backup;
  unsigned long int sensorPhCalcAvg = 0;
  // reading ten samples after 10 ms into the field
  for (int i = 0; i < 10; i++) {
    sensorPhArray[i] = analogRead(PIN_SENSOR_PH);
    delay(10);
  }
  for (int i = 0; i < 10; i++) {
    Serial.print(sensorPhArray[i]);
    Serial.print(" | ");
  }
    Serial.println(" ");
  // sorting the members of the measured results field by size
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (sensorPhArray[i] > sensorPhArray[j]) {
        backup = sensorPhArray[i];
        sensorPhArray[i] = sensorPhArray[j];
        sensorPhArray[j] = backup;
      }
    }
  }
  // save the 2nd to 8th result in variables from which the average is calculated
  // (omit two array members at the beginning and end for better accuracy)
  for (int i = 2; i < 8; i++) {
    sensorPhCalcAvg += sensorPhArray[i];
  }
  // calculation of pH value from average
  // measurement and conversion to the range 0-14 pH
  float sensorPhAvg = (float)sensorPhCalcAvg * 5.0 / 1024 / 6;
  sensorPh = -5.70 * sensorPhAvg + 21.34 + 2;
}
