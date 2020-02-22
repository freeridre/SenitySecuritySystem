#include <Arduino.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "SparkFun_Qwiic_Rfid.h"
#include <Softwareserial.h>
#include <Adafruit_Fingerprint.h>
#include <FastLED.h>
#include "RTClib.h"
#define LED_PIN               30
#define NUM_LEDS              12
#define BRIGHTNESS1           255
#define BRIGHTNESS2           192
#define BRIGHTNESS3           129
#define BRIGHTNESS4           66
#define LED_TYPE              WS2812B
#define COLOR_ORDER           GRB
int fadeAmount = 15;               // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightness = 0;
//#define UPDATES_PER_SECOND    100
//#define FRAMES_PER_SECOND     120
CRGB leds[NUM_LEDS];
int WS2812BTrunOff = 0;
byte FASTLED_FADE_COUNTER = 0;
byte FASTLED_FADE_COUNTER2 = 0;
byte FASTLED_FADE_SPEED = 5;
byte FASTLED_FADE_COUNTER_STORE;
byte LEDCOUNT = 0;
uint8_t hue = 0;
void sinelon();
uint8_t gHue = 50;
SoftwareSerial FingerPrintSerial(12, 13); // RX, TX
Adafruit_Fingerprint FingerPrint = Adafruit_Fingerprint(&FingerPrintSerial);
SoftwareSerial HC05JDY30(10, 11); // RX, TX
//Define Flags
//if Datapackage[2] == 255 Door opened at
//if Datapackage[2] == 254 Door closed at
//if Datapackage[2] == 253 Outside button pushed
//PN532 Configuration
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
int a = 9;
int uidsendtolora = 0;
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength, timeout;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
uint8_t uidDataPackage[] = {255, 255, 0, 0, 0, 0, 0, 0, 0};
byte uidBytesCount = 0;
uint8_t CardExtension [3] = {0, 0, 0};
byte uidLengthplus = 0;
//BUZZER INIT
void BuzzerCarding();
int On = 255, Off = 0, Standby = 50;
int BuzzerOn = 0, BuzzerOff = 255;
int BuzzerFrequency1 = 2000; int BuzzerFrequency2 = 2100; int BuzzerFrequency3 = 2200; int BuzzerFrequency4 = 2300; int BuzzerFrequency5 = 2400;
int Buzzer = 31;
int RedPin = 2;
int GreenPin = 4;
int BluePin = 3;
int MagneticLock = 5;
//Button init
//                              sig, sig, inf
uint8_t ButtonDataPackage[3] = {111, 111, 253};
int Button = 7;
int ButtonVal;
int ButtonValToDoorControl;
int PIR = 8;
int PIRState = 0;
unsigned int PIRCount = 0;
unsigned int PIRStat2 = 0;
unsigned int CountPir = 0;
unsigned int PIRStateControl, ReturnMOTIONPIR;
unsigned int WaitingPIR, WaitingPIR2, WaitingCardReading, WaitingCardReading2, osszeg = 0, CountDownReader = 3;
//RTC DS3231 Configuration
RTC_DS3231 rtc;
uint8_t DS3231Time[] = {0, 0, 0, 0, 0, 0, 0};
uint8_t DS3231TimeCurrentTime[] = {0, 0, 0, 0, 0, 0, 0};
DateTime now;
//LoRa Seetings
uint8_t LORA_RESET[3] = { 111, 111, 111};
//How much time after Motion detect.
unsigned int PowerOnTime = 100;
//MagnetReed
const int Reed = 9;
int RerturnDoorTimeOutCardReading;
int ReedState;
unsigned long SecondTime;
//How much time after opened door
unsigned long WaitingOpenedDoor = 300; //150 = 8 ms; 18 = 1ms
int PrevTimeDoorOpenedMillis = 0;
int CurrentTimeDoorOpenedMillis = 0;
//                                           sig, sig, inf
uint8_t DOORSTATUS__CLOSED_DATAPACKAGE[3] = {111, 111, 254};
uint8_t DOORSTATUS_OPENED_DATAPACKAGE[3] = {111, 111, 255};
//DoorOpenedStat
byte DOORSTATUS = 9;
byte DoorOpenedStatus = 0;
//Enable opened door detection 0 - no, 1 - BuzzerWithLeds, 2 - noBuzzer
#define DOOROPENEDENABLE 1
unsigned long FirstTime;
unsigned long DoorTimeOut = 0;
unsigned long ReturnDoorTimeOut;
unsigned int DoorOpenStatusForWS2812B = 0;
unsigned long FunctionReturnDoorTimeOut;

//ControlData
int ControlData;
#define RFID_ADDR 0x7D // Default I2C address 

Qwiic_Rfid myRfid(RFID_ADDR);

String tag, DataString; 
int IntegerDataString;
int IntegerDataStringArray[16] = {};
int b = 1;
int c = 0;
int RFIDID = 14;
int state = 0;
unsigned int i = 0;
//ControlDataCodes
/*
12 - AccessGranted
11 - AccessDenied
10 - OutsideButtonPushed
*/
//TIMER INIT
unsigned long currentMillis;
unsigned long time_half_second = 5000;
void TimeStamp();
void TimeNow();
boolean cardreading ();
void NFCINITIALIZE ();
void senddatatolora();
void ButtonPushedOutSide();
void HC05JDY30FUNCTION();
void CountDownTimerPIR();
void LedBuzzerOpenedGateTimeOut();
void LoRaReset ();
int DoorOpenedTimeOut(int ReturnDoorTimeOut);
int MOTIONPIR(int PIRStateControl);
int OpenedGateTimeOutOn = 400;
int OpenedGateTimeOutOff = 400;
int LedBuzzerOpenedGateTimeOutV2 = 0;
unsigned long previousMillis = 0;
unsigned long OpenedDoorOnTime = 250;
unsigned long OpenedDoorOffTime = 250;
unsigned long OffTime = 1000;
boolean DoorOpenState = false;
void WS2812BYellow2();
void LoRaReset ()
{
  Serial1.write(LORA_RESET, sizeof(LORA_RESET));
  Serial.println("LoRa Has Just Reseted. Cause: Arduino Gate Has Reseted.");
  //Serial1.flush();
}
void WS2812BBlack()
{
  for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Black;  // Set Color HERE!!!
  }
}
void WS2812BRed()
{
  for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Red;  // Set Color HERE!!!
  }
}
void WS2812BGreen()
{
  for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Green;  // Set Color HERE!!!
  }
}
void WS2812BYellow()
{
  for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Yellow;  // Set Color HERE!!!
  }
  FastLED.show();
}
void WS2812BYellow2()
{
   for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Yellow;  // Set Color HERE!!!
    leds[i].fadeLightBy(brightness);
  }
  FastLED.show();
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount ; 
  }    
  delay(50);
}
void WS2812BBlue()
{
  //FastLED.setBrightness(BRIGHTNESS1);
  //fill_solid(leds, NUM_LEDS, CHSV(hue, 0, 255));
//FastLED_fade_in_counter = 0;
//for (int i = 0; NUM_LEDS > i; i++)
  //{
  //leds[i] = CRGB::Blue;
/*for(FastLED_fade_in_counter = 0; FastLED_fade_in_counter < 50; FastLED_fade_in_counter = +10)
    {
      //leds[i].maximizeBrightness(FastLED_fade_in_counter);
      //FastLED_fade_in_counter++;
      FastLED.delay(60);
      leds[0] = CRGB::Blue; leds[0].maximizeBrightness(FastLED_fade_in_counter);
      leds[1] = CRGB::Blue; leds[1].maximizeBrightness(FastLED_fade_in_counter);
      leds[2] = CRGB::Blue; leds[2].maximizeBrightness(FastLED_fade_in_counter);
      leds[3] = CRGB::Blue; leds[3].maximizeBrightness(FastLED_fade_in_counter);
      leds[4] = CRGB::Blue; leds[4].maximizeBrightness(FastLED_fade_in_counter);
      leds[5] = CRGB::Blue; leds[5].maximizeBrightness(FastLED_fade_in_counter);
      leds[6] = CRGB::Blue; leds[6].maximizeBrightness(FastLED_fade_in_counter);
      leds[7] = CRGB::Blue; leds[7].maximizeBrightness(FastLED_fade_in_counter);
      leds[8] = CRGB::Blue; leds[8].maximizeBrightness(FastLED_fade_in_counter);
      leds[9] = CRGB::Blue; leds[9].maximizeBrightness(FastLED_fade_in_counter);
      leds[10] = CRGB::Blue; leds[10].maximizeBrightness(FastLED_fade_in_counter);
      leds[11] = CRGB::Blue; leds[11].maximizeBrightness(FastLED_fade_in_counter);
    }*/

///////////////////////////////////////////////////////////////////////////////////////////////////
  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Blue;
    leds[i].maximizeBrightness(FASTLED_FADE_COUNTER);
  }
  FASTLED_FADE_COUNTER = + FASTLED_FADE_SPEED;
  if (FASTLED_FADE_COUNTER == 255)
  {
    FASTLED_FADE_COUNTER = 0;
  }
  FastLED.show();
///////////////////////////////////////////////////////////////////////////////////////////////////
for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Blue;  // Set Color HERE!!!
    leds[i].fadeLightBy(brightness);
    
  }
  FastLED.show();
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount ; 
    FASTLED_FADE_COUNTER = 0;
  }    
  delay(50);



















///////////////////////////////////////////////////////////////////////////////////////////////////
  /*for(int i = 0; i < NUM_LEDS; i++ )
   {
    leds[i] = CRGB::Blue;  // Set Color HERE!!!
    leds[i].fadeLightBy(brightness);
    
  }
  FastLED.show();
  brightness = brightness + fadeAmount;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness == 0 || brightness == 255)
  {
    fadeAmount = -fadeAmount ; 
  }    
  delay(50);*/  // This delay sets speed of the fade. I usually do from 5-75 but you can always go higher.
///////////////////////////////////////////////////////////////////////////////////////////////////



  //dim8_raw(leds);

  /*if(FastLED_fade_in_counter == 50)
    {*/
      //FastLED_fade_in_counter = 0;
      //FastLED.clear(leds);
    //}
  //}
  
  
}
void sinelon()
{
    FastLED.setBrightness(BRIGHTNESS1);
    fadeToBlackBy(leds,NUM_LEDS, 150);
    int pos = beatsin16(17, 0, NUM_LEDS-1);
    leds[pos] = (0, 0, 255);
}
void setColor(int Red, int Green, int Blue)
{
  #ifdef COMMON_ANODE
    Red = 255 - RedPin;
    Green = 255 - GreenPin;
    BluePin = 255 - BluePin;
  #endif
  analogWrite(RedPin, Red);
  analogWrite(GreenPin, Green);
  analogWrite(BluePin, Blue);
  
}
int MOTIONPIR(int PIRStateControl)
{
  PIRState = digitalRead(PIR);
  if (PIRState == LOW)
  {
    //Serial.println("No Motion!");
    PIRStateControl = 0;
    CountPir++;
    return PIRStateControl;
  }
  else if (PIRState == HIGH)
  {
    Serial.println("Motion Detected!");
    PIRStateControl = 1;
    return PIRStateControl;
  }
  //return 1;
}
void LedBuzzerOpenedGateTimeOut()
{
  //Serial.print("Current Millis: "); Serial.println(currentMillis);
  if((DoorOpenState) && (currentMillis - previousMillis) >= OpenedDoorOnTime)
  {
    //unsigned long ubc = currentMillis - previousMillis;
    //Serial.print("Current Millis - Previous Millis: "); Serial.println(ubc);
    //ledState = LOW;  // Turn it off
    previousMillis = currentMillis;  // Remember the time
    //Serial.print("Previous Millis: "); Serial.println(previousMillis);
    //digitalWrite(ledPin, ledState);  // Update the actual LED
    //analogWrite(Buzzer, BuzzerOff);
    
    setColor(Off, Off, Off);
    //FastLED.clear(leds);
    //Serial.println("AJJASJF");
    DoorOpenState = false;
    //Release Motion RGB RING Effect (Fades)
    PIRStat2 = 0;
  }
  else if ((!DoorOpenState) && (currentMillis - previousMillis) >= OpenedDoorOffTime)
  {
    //ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    //digitalWrite(ledPin, ledState);	  // Update the actual LED
    //analogWrite(Buzzer, BuzzerOn);
    if (DOOROPENEDENABLE == 1)
    {
      tone(Buzzer, BuzzerFrequency1);
    }
    setColor(On, On, Off);
    WS2812BYellow();
    //Serial.println("125235163262");
    DoorOpenState = true;
    //Release Motion RGB RING Effect (Fades)
    PIRStat2 = 0;
  }
  /*setColor(On, On, Off);
  analogWrite(Buzzer, BuzzerOn);
  delay(250);
  setColor(Off, Off, Off);
  analogWrite(Buzzer, BuzzerOff);
  delay(250);*/
  //Release Motion RGB RING Effect (Fades)
  PIRStat2 = 0;
  FastLED.clear();
}
void LedBuzzerAccessGranted()
{
  setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  WS2812BGreen();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  tone(Buzzer, BuzzerFrequency1);
  delay(150);

  setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  noTone(Buzzer);
  delay(150);

  setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  WS2812BGreen();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  tone(Buzzer, BuzzerFrequency1);
  delay(1000);

  setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  noTone(Buzzer);
  delay(150);
  //Blue
  setColor(Off, Off, On);
  //Release Motion RGB RING Effect (Fades)
  PIRStat2 = 0;
}
void LedBuzzerAccessDenied()
{
  int ledx = 0;
  for (ledx = 0; ledx < 13; ledx++)
  {
    leds[ledx] = (255, 0, 0);
  }
setColor(On, Off, Off);
WS2812BRed();
FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  tone(Buzzer, BuzzerFrequency1);
  delay(150);
  setColor(Off, Off, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  noTone(Buzzer);
  delay(150);
  //Blue
  setColor(Off, Off, On);
  //Release Motion RGB RING Effect (Fades)
  PIRStat2 = 0;
}
void setup()
{
  FirstTime = millis();
  FastLED.setBrightness(BRIGHTNESS1);
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear(leds);
  
  //FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS);
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(MagneticLock, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  //analogWrite(Buzzer, BuzzerOn);
  tone(Buzzer, BuzzerFrequency1);
  pinMode(Button, INPUT);
  pinMode(PIR, INPUT);
  pinMode(Reed, INPUT_PULLUP);
  delay(100);
  if (currentMillis < time_half_second)
  {
    //analogWrite(Buzzer, BuzzerOff);
    noTone(Buzzer);
    setColor(Off, Off, Standby);
    }
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Hello!");
  if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }else
  Serial.println("RTC Has just Started!");
  TimeNow();
  LoRaReset();
  delay(2000);
  //**Wire.begin();
  HC05JDY30.begin(9600);
  if(HC05JDY30)
  {
    Serial.println("HC-05 is ready to use!");
  }
  else
  {
    Serial.println("Could not communicate with HC-05!");
  }
  //FingerPrint.begin(57600);
  //FingerPrintCheck();
  /**if(myRfid.begin())
    Serial.println("RFID is ready to use!"); 
  else
    Serial.println("Could not communicate with Qwiic RFID!");**/
  NFCINITIALIZE ();
  //cardreading ();
}
void RecieveDataFromGateLoRa()
{
  if(Serial1.available() > 0)
  {
    ControlData = Serial1.read();
    Serial.println(ControlData);
    if(ControlData == 12)
    {
      LedBuzzerAccessGranted();
      Serial.println("Access Granted, door has just opened!");
    }
    if (ControlData == 11)
    {
      for (int i = 0; i < 3; i++)
      {
        LedBuzzerAccessDenied();
      }
      Serial.println("Access Denied!");
    }
    Serial.println("Waiting for an ISO14443A Card ...");
  }
}
void HC05JDY30FUNCTION()
{
  if(HC05JDY30.available() > 0)
  { // Checks whether data is comming from the serial port
    state = HC05JDY30.read(); // Reads the data from the serial port
    if (state == 1)
    {
      LedBuzzerAccessGranted();
      Serial.println("Access Granted, door has just opened by SmartPhoneApp!");
    }
  }
}
int DoorOpenedTimeOut(int ReturnDoorTimeOut)
{
  /////////////////////////////////////////////////////////////////////
  if (DOOROPENEDENABLE == 1)
  {  
    ReedState = digitalRead(Reed);
    //DOORSTATUS = 9;
    if (ReedState == HIGH)
    {
      
      if (DoorOpenStatusForWS2812B != 1 )
      {
        sinelon();
        
      }
      ReedState = digitalRead(Reed);
      DoorTimeOut++;
      /*Serial.print(DoorTimeOut);*/
      /*CurrentTimeDoorOpenedMillis = millis();
      Serial.print("Current Millis: "); Serial.println(millis());*/
      //if (CurrentTimeDoorOpenedMillis >= WaitingOpenedDoor)
      Serial.println(DoorTimeOut);
      if (DoorTimeOut >= WaitingOpenedDoor)
      {
        //PrevTimeDoorOpenedMillis = CurrentTimeDoorOpenedMillis;
        //Nem fog túlcsordulni
        if(DOORSTATUS >= 10)
        {
          DOORSTATUS = 10;
        }else if (DOORSTATUS <= 8)
        {
          DOORSTATUS = 9;
        }
        //DoorOpenedStatus = 1;
        //Serial.println("The Door is Opened!");
        ReedState = digitalRead(Reed);
        DoorTimeOut--;
        //DoorTimeOut = 0;
        DoorOpenStatusForWS2812B = 1;
        LedBuzzerOpenedGateTimeOut();
        DOORSTATUS++;
        //Serial.print("When it's Opened1: ");Serial.println(DOORSTATUS);
        //delay(100);
        return ReturnDoorTimeOut = 1;
        /*if (ReedState == LOW)
        {
          DoorTimeOut = 0;
          Serial.print(DoorTimeOut); Serial.println("Released");
          setColor(Off, Off, Standby);
        }*/
      }
      ReturnDoorTimeOut = 2;
    }
    else if (ReedState == LOW)
    {
      //Serial.print("1st: ");Serial.println(DOORSTATUS);

      if(DOORSTATUS >= 10)
      {
        DOORSTATUS = 8;
      }
      /*else if (DOORSTATUS < 8)
      {
        DOORSTATUS = 8;
        
      }else if(DOORSTATUS != 9)
      {
        DOORSTATUS--;
      }*/
      
      //Serial.print("When it's Closed1: ");Serial.println(DOORSTATUS);
      //Serial.println(DOORSTATUS);
      //Serial.println("The Door is Closed!");
      DoorOpenedStatus = 0;
      //ReedState = digitalRead(Reed);
      DoorTimeOut = 0;
      DoorOpenStatusForWS2812B = 0;
      //sinelon();
      //analogWrite(Buzzer, BuzzerOff);

      //noTone(Buzzer);

      /*Serial.print(DoorTimeOut);
      Serial.println(" Locked");*/
      return ReturnDoorTimeOut = 0;
    }
  /////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////
  }else if (DOOROPENEDENABLE == 0)
  {
  /////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////
  }else
  {
    ReedState = digitalRead(Reed);
    if (ReedState == HIGH)
    {
      if (DoorOpenStatusForWS2812B != 1)
      {
      sinelon();
      }
      ReedState = digitalRead(Reed);
      DoorTimeOut++;
      /*Serial.print(DoorTimeOut);*/
      Serial.println(" Opened");
      if (DoorTimeOut >= WaitingOpenedDoor)
      {
        ReedState = digitalRead(Reed);
        DoorTimeOut--;
        DoorOpenStatusForWS2812B = 1;
        LedBuzzerOpenedGateTimeOut();

        return ReturnDoorTimeOut = 1;
        /*if (ReedState == LOW)
        {
          DoorTimeOut = 0;
          Serial.print(DoorTimeOut); Serial.println("Released");
          setColor(Off, Off, Standby);
        }*/
      }
    }
    else if (ReedState == LOW)
    {
      ReedState = digitalRead(Reed);
      DoorTimeOut = 0;
      setColor(Off, Off, Standby);
      //sinelon();
      //analogWrite(Buzzer, BuzzerOff);

      //noTone(Buzzer);

      /*Serial.print(DoorTimeOut);
      Serial.println(" Locked");*/
      return ReturnDoorTimeOut = 0;
    }
  }
  /////////////////////////////////////////////////////////////////////
}
void loop()
{
  noTone(Buzzer);
  //FastLED.delay(1000/FRAMES_PER_SECOND);
  //EVERY_N_MILLISECONDS(100);
  //**Serial.println(CountPir);
  currentMillis = millis();
  MOTIONPIR(PIRStateControl);
  DoorOpenedTimeOut(ReturnDoorTimeOut);
  ReturnMOTIONPIR = MOTIONPIR(PIRStateControl);
  //Serial.print("ReturnMOTIONPIR: "); Serial.println(ReturnMOTIONPIR);
  int ReturnReturnDoorTimeOut = DoorOpenedTimeOut(ReturnDoorTimeOut);
  //Closed Door
  if(ReturnReturnDoorTimeOut == 0)
  {
    //Serial.print("When it's Closed2: ");Serial.println(DOORSTATUS);
    /*if (ReturnMOTIONPIR == 0)
    {
      //WaitingPIR = millis();
      //Serial.println("Motion Detected!");
      //Serial.println(WaitingPIR);
      setColor(Off, Off, On);
      PIRState = digitalRead(PIR);
      Serial.print("PIRState: "); Serial.println(PIRState);
      Serial.print("PIRCount: "); Serial.println(PIRCount);
      PIRCount++;
      if (PIRCount >= PowerOnTime)
      {
        PIRState = digitalRead(PIR);
        PIRCount--;
        setColor(Off, Off, Standby);
        Serial.print("PIRCount: "); Serial.println(PIRCount);
      }
    }
    WaitingPIR2 = millis();
    if(WaitingPIR2 - WaitingPIR > PowerOnTime)
    {
      Serial.print("Waiting PIR2: ");Serial.println(WaitingPIR2);
      Serial.print("Waiting PIR: ");Serial.println(WaitingPIR);
    setColor(Off, Off, Standby);
    Serial.println("No Motion!");
    WaitingPIR2 = 0;
    WaitingPIR = 0;
    }*/
//Serial.print("Zarva!"); Serial.println(DOORSTATUS);
  if (DOORSTATUS == 8)
      {
        Serial.print("The Door is Closed at: ");
        TimeStamp();
        Serial1.write(DOORSTATUS__CLOSED_DATAPACKAGE, sizeof(DOORSTATUS__CLOSED_DATAPACKAGE));
        Serial1.write(DS3231Time, sizeof(DS3231Time));
        DOORSTATUS--;
      }

    if (ReturnMOTIONPIR == 1)
    {
      PIRStat2 = 1;
    }
    if (PIRStat2 == 1)
    {
      
      //Serial.println("Itt2!");
      setColor(Off, Off, On);
      WS2812BBlue();
      WS2812BTrunOff = 1;
      PIRCount++;
      //Serial.print("PIRCount: "); Serial.println(PIRCount);
      if (PIRCount >= PowerOnTime)
      {
        PIRState = digitalRead(PIR);
        PIRCount--;
        //setColor(Off, Off, Standby);
        
        //Serial.println("Itt!");
        //Serial.print("PIRCount: "); Serial.println(PIRCount);
        PIRStat2 = 0;
        PIRCount = 0;
      }
    }
    else if (ReturnMOTIONPIR == 0)
    {
      PIRStat2 = 0;
    }
    if (PIRStat2 == 0)
    {
      sinelon();
    }
    //Opened Door
  }else if (ReturnReturnDoorTimeOut == 1)
  {
    DoorOpenedStatus = 0;
    //Serial.print("When it's Opened2: ");Serial.println(DOORSTATUS);
    if (DOORSTATUS == 10)
    {
      Serial.print("The Door is Opened at: ");
      TimeStamp();
      Serial1.write(DOORSTATUS_OPENED_DATAPACKAGE, sizeof(DOORSTATUS_OPENED_DATAPACKAGE));
      Serial1.write(DS3231Time, sizeof(DS3231Time));
    }
  }
  if(cardreading())
    {
      Serial.print("Carding at: ");
      TimeStamp();
      senddatatolora();
    }
  RecieveDataFromGateLoRa();
  ButtonPushedOutSide();
  HC05JDY30FUNCTION();
  
  FastLED.show();
  
}
void NFCINITIALIZE ()
{

nfc.begin();
nfc.setPassiveActivationRetries(0x01);

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.println("Waiting for a Card...");
}
  boolean cardreading ()
{
  //Serial.println("ITT2");
  boolean ret = false;
  /*MOTIONPIR(PIRStateControl);
  ReturnMOTIONPIR = MOTIONPIR(PIRStateControl);
  if (ReturnMOTIONPIR == 1)
  {*/
  //*****************************************************************
  //Innentől működő RFID 125KHZ olvasás, átalakítása 4byte UID
      /**tag = myRfid.getTag();
      if(tag.toInt() < 0)
      {
        unsigned int TagLength = tag.length();
        while (TagLength < 16)
        {
          for (i; i < TagLength; i++)
          {
            DataString = tag.substring(c, b);
            IntegerDataStringArray[i] = DataString.toInt();
            b++;
            c++;
          }
          while (TagLength < 16)
          {
            IntegerDataStringArray[i] = RFIDID;
            TagLength++;
            i++;
            RFIDID--;
          }
        }
        if (TagLength == 16)
        {
          for (unsigned int u = 0; u < TagLength; u++)
          {
            if (u <= 3)
            {
              uid[0] += IntegerDataStringArray[u];
            }
            if (u <= 7 && u > 3)
            {
              uid[1] += IntegerDataStringArray[u];
            }
            if (u <= 11 && u > 7)
            {
              uid[2] += IntegerDataStringArray[u];
            }
            if (u <= 15 && u > 11)
            {
              uid[3] += IntegerDataStringArray[u];
            }
          }
        }
        //Release variables
        b = 1;c = 0;i = 0;RFIDID = 14;
        Serial.print("TAG ID: ");
        for (int p = 0; p < 4; p++)
        {
        Serial.print("0x");Serial.print(uid[p]);Serial.print(" ");
        uidLength++;
        }
        Serial.println();
        if (uid[0] > 0)
        {
          if (uid[1] > 0)
          {
            if (uid[2] > 0)
            {
              if (uid[3] > 0)
              {
                ret = true;
              }
            }
          }
        }
        else if (uid[1] <= 0)
        {
          ret = false;
        }
        for (unsigned int u; u < TagLength; u++)
        {
          IntegerDataStringArray[u] = 0;
        }
        TagLength = 0;
        tag = "";
        return ret;
      }**/
      //*****************************************************************************************
    // configure board to read RFID tags
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    nfc.SAMConfig();
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);//, timeout = 50);
    // Display some basic information about the card
    if (uid[1] > 0)
      {
        BuzzerCarding();
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        //Serial.println("");

        /*uidBytesCount = 0;
        for(int hu = 0; hu <sizeof(uidLength); hu++)
        {
          if(uid[hu] > 0)
          {
            uidBytesCount++;
          }
        }
        Serial.print("The uid contains: "); Serial.print(uidBytesCount); Serial.println(" bytes.");*/

        while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength))
        {
          RerturnDoorTimeOutCardReading = DoorOpenedTimeOut(ReturnDoorTimeOut);
          //ha nincs nyitva van, akkor legyen kek feny
          //Serial.print("Door Return Byte: "); Serial.println(RerturnDoorTimeOutCardReading);
          if (RerturnDoorTimeOutCardReading == 0)
          {
            //Serial.println("The Door is Closed 2!");
            WS2812BBlue();
            RerturnDoorTimeOutCardReading = 2;
            

            //ha nyitva van, akkor ne legyen kek feny
          }else if (RerturnDoorTimeOutCardReading == 1)
          {
            //Serial.println("The Door is Opened 2!");
            //LedBuzzerOpenedGateTimeOut();
            WS2812BYellow2();
            RerturnDoorTimeOutCardReading = 2;
          }
            WS2812BBlue();
        }
        /*while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength))
        {
          RerturnDoorTimeOutCardReading = DoorOpenedTimeOut(ReturnDoorTimeOut);
          //ha nincs nyitva van, akkor legyen kek feny
          //Serial.print("Door Return Byte: "); Serial.println(RerturnDoorTimeOutCardReading);
          if (RerturnDoorTimeOutCardReading == 0)
          {
            //Serial.println("The Door is Closed 2!");
            WS2812BBlue();
            RerturnDoorTimeOutCardReading = 2;
            

            //ha nyitva van, akkor ne legyen kek feny
          }else if (RerturnDoorTimeOutCardReading == 1)
          {
            //Serial.println("The Door is Opened 2!");
            //LedBuzzerOpenedGateTimeOut();
            WS2812BYellow2();
            RerturnDoorTimeOutCardReading = 2;
          }
          /*else
            WS2812BBlue();
        }*/
        //Serial.print("UIDLength: "); Serial.println(uidLength);
        if(uidLength == 4)
        {
          uidLengthplus = uidLength+2;
        }else if (uidLength == 7)
        {
          uidLengthplus = uidLength+2;
        }
        
        for(int h = 2; h < uidLengthplus; h++)
        {
          uidDataPackage[h] = uid[uidBytesCount];
          uidBytesCount++;
          //Serial.print(uidDataPackage[h], HEX);Serial.print(" ");
        }
        Serial.println();
        /*Serial.print("DataPackage is: ");
        for(int hu = 0; hu < sizeof(uidDataPackage); hu++)
        {
          Serial.print(uidDataPackage[hu], HEX);Serial.print(" ");
        }
        Serial.println();*/

        uidBytesCount = 0;
        ret = true;
      }
    else if (uid[1] == 0)
    {
      ret = false;
    }
  /*}
  else if (ReturnMOTIONPIR == 0)
  {
    Serial.println("Itt vagyok");
    ReturnMOTIONPIR = 0;
    ret = false;
    return ret;
  }*/
  return ret;
}
  void senddatatolora()
  {
    //Serial1.write(uid, uidLength);
    Serial1.write(uidDataPackage, sizeof(uidDataPackage));
    /*if (uidLength == 4)
    {
      Serial1.write(CardExtension, sizeof(CardExtension));
    }*/
    Serial1.write(DS3231Time, sizeof(DS3231Time));
    /*Serial.print("Uid 1st byte: "); Serial.println(uid[0], HEX);
    Serial.print("Uid 2nd byte: "); Serial.println(uid[1], HEX);
    Serial.print("Uid 3th byte: "); Serial.println(uid[2], HEX);
    Serial.print("Uid 4th byte: "); Serial.println(uid[3], HEX);
    Serial.print("Uid length: "); Serial.println(uidLength);*/
    Serial.println("UID, and TimeStamp HAS SENT TO LORA!");
    for (unsigned int i = 0; i < sizeof(uid); i++)
    {
      uid[i] = 0;
    }
    uidLength = 0;
    Serial.println("UID, and UIDLength HAS Been Cleared!");
    for (unsigned int ib = 2; i < sizeof(uidDataPackage); i++)
    {
      uidDataPackage[ib] = 0;
    }
    
    Serial.println("UidDataPackage HAS Been Cleared!");
    for (unsigned int i = 0; i < sizeof(uid); i++)
    {
      //int UIDDelete = uid[i];
      Serial.print("0x");
      Serial.print(uid[i], HEX);
    }
    Serial.print("\n");
    for (unsigned int ib = 0; i < sizeof(uidDataPackage); i++)
    {
      //int UIDDelete = uidDataPackage[ib];
      Serial.print("0x");
      Serial.print(uidDataPackage[ib], HEX);
    }
    Serial.print("\n");
    Serial.println();
    Serial.println("Waiting for a card...");
    //delay(1110);
    }
void ButtonPushedOutSide ()
{
  ButtonVal = digitalRead(Button);
  if(ButtonVal == LOW)
  {
    Serial.println("Button has just pushed!");
    ButtonValToDoorControl = 1;
    if (ButtonValToDoorControl == 1)
    {
        ControlData = 10;
        LedBuzzerAccessGranted();
        Serial.println("Access Granted, door has just opened by Buttonat: ");
        TimeStamp();

        Serial1.write(ButtonDataPackage, sizeof(ButtonDataPackage));
        Serial1.write(DS3231Time, sizeof(DS3231Time));
        Serial.println("Waiting for an ISO14443A Card ...");
        ButtonValToDoorControl = 0;
    }
  }
}
void TimeNow()
{
  now = rtc.now();
    if (rtc.lostPower())
    {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    now = rtc.now().unixtime();
  }
  Serial.print("Current Date And Time: ");
    Serial.print(now.year(), DEC);
    Serial.print(".");
    if(now.month() < 10)
    {
        Serial.print("0");
    }
    Serial.print(now.month(), DEC);
    Serial.print(".");
    if(now.day() < 10)
    {
        Serial.print("0");
    }
    Serial.print(now.day(), DEC);
    Serial.print(". ");
    Serial.print("(");
    Serial.print(now.dayOfTheWeek(), DEC);
    Serial.print(") ");
    if(now.hour() < 10)
    {
        Serial.print("0");
    }
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    if(now.minute() < 10)
    {
        Serial.print("0");
    }
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    if(now.second() < 10)
    {
        Serial.print("0");
    }
    Serial.println(now.second(), DEC);
    //delay(1000);
}
void TimeStamp()
{
    now = rtc.now();
    //Serial.println("Time Stamp: ");
    DS3231Time[0] = now.year()-2000; DS3231Time[1] = now.month(); DS3231Time[2] = now.day();
    DS3231Time[3] = now.dayOfTheWeek(); DS3231Time[4] = now.hour(); DS3231Time[5] = now.minute(); DS3231Time[6] = now.second();
    //Year
    Serial.print(DS3231Time[0]);
    Serial.print(".");
    //Month
    if(DS3231Time[1] < 10)
    {
        Serial.print("0");
    }
    Serial.print(DS3231Time[1], DEC);
    Serial.print(".");
    //Day
    if(DS3231Time[2] < 10)
    {
        Serial.print("0");
    }
    Serial.print(DS3231Time[2], DEC);
    Serial.print(". ");
    //Day of the week
    Serial.print("(");
    Serial.print(DS3231Time[3], DEC);
    Serial.print(") ");
    //Hour
    if(DS3231Time[4] < 10)
    {
        Serial.print("0");
    }
    Serial.print(DS3231Time[4], DEC);
    Serial.print(":");
    //Minute
    if(DS3231Time[5] < 10)
    {
        Serial.print("0");
    }
    Serial.print(DS3231Time[5], DEC);
    Serial.print(":");
    //Second
    if(DS3231Time[6] < 10)
    {
        Serial.print("0");
    }
    Serial.println(DS3231Time[6], DEC);
}
void BuzzerCarding()
{
  tone(Buzzer, BuzzerFrequency1);
  delay(100);
  noTone(Buzzer);
  delay(100);
}
