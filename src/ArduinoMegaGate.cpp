#include <Arduino.h>
#include <avr/wdt.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "SparkFun_Qwiic_Rfid.h"
#include <Softwareserial.h>
#include <EEPROM.h>
//#include <Adafruit_Fingerprint.h>
#include <FastLED.h>
#include <RTClib.h>
//#include <Nextion.h>
#define LED_PIN               30
#define NUM_LEDS              12
#define BRIGHTNESS1           255
#define BRIGHTNESS2           192
#define BRIGHTNESS3           129
#define BRIGHTNESS4           66
#define BRIGHTNESSOFF         0
#define LED_TYPE              WS2812B
#define COLOR_ORDER           GRB
int fadeAmount = 15;               // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightness = 0;
boolean LedStatus = false;
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
//Adafruit_Fingerprint FingerPrint = Adafruit_Fingerprint(&FingerPrintSerial);
SoftwareSerial HC05JDY30(10, 11); // RX, TX
//Define Flags
//if Datapackage[2] == 255 Door opened at
//if Datapackage[2] == 254 Door closed at
//if Datapackage[2] == 253 Outside button pushed
//HC05BlueToothconf
uint8_t HC05Status[3] = {111, 111, 99};
//PN532 Configuration
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
uint8_t NFCbuf[128];
int a = 9;
int uidsendtolora = 0;
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength, timeout;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
uint8_t uidDataPackage[] = {255, 255, 0, 0, 0, 0, 0, 0, 0};
uint8_t SecurityDataPackage[] = {111, 111, 20, 0, 0, 0, 0, 0, 0};
byte uidBytesCount = 0;
uint8_t CardExtension [3] = {0, 0, 0};
byte uidLengthplus = 0;
unsigned int Ardueeprom_all_Card_data_bytes_cont, Ardueeprom_card_num_cont;
//BUZZER INIT
void BuzzerCarding();
boolean BuzzerON;
byte BuzzerPreConf;

int On = 255, Off = 0, Standby = 50;
//int BuzzerOn = 0, BuzzerOff = 255;
int BuzzerFrequency1 = 2000; int BuzzerFrequency2 = 2100; int BuzzerFrequency3 = 2200; int BuzzerFrequency4 = 2300; int BuzzerFrequency5 = 2400;
int BuzzerLowFreq1 = 1500;
int Buzzer = 31;
//EEPROM_RGB_PINOUTS
int REDLIGHT_PIN = 2;
int GRRENLIGHT_PIN = 3;
int BLUELIGHT_PIN = 4;
//EEPROM_24AA1025/24LC1025/24FC1025EEPROM
//if A0, A1, A2 to GND and
#define EEPROM_ADDR_1 0x50
#define EEPROM_ADDR_2 0x54
boolean EEPROM_BLINK_BOOL = true;
byte EEPROM_BLINK_COLOR_CHANGE = 0;
int EEPROM_value = 0;
int EEPROM_index = 0;
unsigned long EEPROM_Size = 127999;
void SuccessProgramingBuzzer();
uint8_t myData[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
#define chipAddress 80
unsigned cellAddress;
void writeTo(int chAddress, unsigned int ceAddress, byte wData);
byte readFrom(int chAddress, unsigned int ceAddress);
void EEPROMReadOutAll();
void EEPROMDeleteAll();
void EEPROM_RGB(int greenlight_value, int bluelight_value, int redlight_value);
void EEPROM_RGB_BLINK();
void EEPROM_RGB_FINISH();
void ACCOffline();
void EEPROM_Vansiher();
void RTCAdjustNextion();
unsigned long EEPROM_Current_TIME = 0;
unsigned long EEPROM_Passed_TIME = 0;
unsigned long EEPROM_TIME_DIF = 50;
byte RGB_index = 0;
void writeTo(int chAddress, unsigned int ceAddress, byte wData);
byte readFrom(int chAddress, unsigned long ceAddress);
void EEPROMReadOutAll();
void EEPROMDeleteAll();
void EEPROM_RGB(int redlight_value, int greenlight_value, int bluelight_value);
void EEPROM_RGB_BLINK();
boolean EEPROM_RGB_BLINK_BOOL = false;
void EEPROM_RGB_FINISH();
unsigned long eeprom_Card_Num = 127900;
unsigned long eeprom_All_Card_Data_Bytes = 126900;
unsigned long eeprom_Card_Num_Cont = 0;
unsigned long eeprom_All_Card_Data_Bytes_Cont;
boolean Card_data_download = false;
unsigned long EEPROM_TIME_OUT = millis();
unsigned long EEPROM_TIME_WAIT = 0;
unsigned long EEPROM_TIME_INTERVAL = 15000;
long EEPROM_T1;
long EEPROM_T2;
long EEPROM_T3;
long EEPROM_T4;
int MagneticLock = 5;
unsigned long Cardi = 2;
unsigned long CardNumberStore = 0;
//Button init
//                              sig, sig, inf
//const uint8_t ButtonDataPackage[3] = {111, 111, 253};
const uint8_t ButtonDataPackage[9] = {111, 111, 21, 0, 0, 0, 0, 0, 0};
const uint8_t RetryCardDataDownloadDataPackage[9] = {111, 111, 22, 0, 0, 0, 0, 0, 0};
const uint8_t MasterUserDP[9] = {111, 111, 23, 0, 0, 0, 0, 0, 0};
const uint8_t InvaildSingInGateDP[9] = {111, 111, 24, 0, 0, 0, 0, 0, 0};
const uint8_t MasterUserDPQuit[9] = {111, 111, 25, 0, 0, 0, 0, 0, 0};
int Button = 7;
int ButtonVal;
int ButtonValToDoorControl;
int PIR = 33;
boolean PIRON = true;
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
unsigned int PowerOnTime = 10;
//MagnetReed
const int Reed = 9;
int RerturnDoorTimeOutCardReading;
int ReedState;
unsigned long SecondTime;
boolean ReedStatusOn = false;
//How much time after opened door
unsigned long WaitingOpenedDoor = 300; //150 = 8 ms; 18 = 1ms
int PrevTimeDoorOpenedMillis = 0;
int CurrentTimeDoorOpenedMillis = 0;
//                                           sig, sig, inf
uint8_t DOORSTATUS__CLOSED_DATAPACKAGE[9] = {111, 111, 254, 0, 0, 0, 0, 0, 0};
uint8_t DOORSTATUS_OPENED_DATAPACKAGE[9] = {111, 111, 255, 0, 0, 0, 0, 0, 0};
//DoorOpenedStat
byte DOORSTATUS = 9;
byte DoorOpenedStatus = 0;
//Enable opened door detection 0 - no, 1 - BuzzerWithLeds, 2 - noBuzzer
byte DOOROPENEDENABLE = 1;
unsigned long FirstTime;
unsigned long DoorTimeOut = 0;
unsigned long ReturnDoorTimeOut;
unsigned int DoorOpenStatusForWS2812B = 0;
unsigned long FunctionReturnDoorTimeOut;
void EEPROM_TIME_OUT_F();
//ControlData
uint8_t ControlData[6] = {0, 0, 0, 0, 0, 0};
int CTD = 0;
#define RFID_ADDR 0x7D // Default I2C address 

Qwiic_Rfid myRfid(RFID_ADDR);

String tag, DataString; 
int IntegerDataString;
int IntegerDataStringArray[16] = {};
int b = 1;
int c = 0;
int RFIDID = 14;
int state = 0;
unsigned int iRFID = 0;
//ControlDataCodes
/*
12 - AccessGranted
11 - AccessDenied
10 - OutsideButtonPushed
*/
//TIMER INIT
unsigned long currentMillis;
unsigned long time_half_second = 5000;
//INIT NEXTION
unsigned long NextionCurrentTime;
unsigned long NextionStartTime;
unsigned int NextionTimeInterval = 1000;
#define NextBuzzer 32
uint8_t NextionBuffer[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NextionOutSideDataPackage[3] = {111, 111, 252};
int NextionI = 0;
boolean NextionDataRec = false;
byte NextionReadFromArduEEPROM;
byte NextionWriteToArduEEPROM;
void ReadFromNextion();
void BuzzNextionSuccess();
void BuzzNextionDenied();
void NextionBuzzer();
void SendTimeToNextion();
void TimeToNextion();
void NextionPassCodeTimeOut();
void NextionBuzzerQuit();
void NextionWriteArduinoEEPROM();
void NextionReadArduinoEEPROM();
void NextionReadArduinoEEPROM2();
void InsideEEPROM_Conf();
void SendEEPROM_conf_to_Nextion();

void TimeStamp();
void TimeNow();
void TIMEAdjust();
boolean cardreading ();
void NFCINITIALIZE ();
void senddatatolora();
void ButtonPushedOutSide();
void HC05JDY30FUNCTION();
void CountDownTimerPIR();
void LedBuzzerOpenedGateTimeOut();
void NoSuppBuzzer();
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

void CardfromDBToEEPROM();

//byte RSTARDU = 5;
//void RSTArduino();
void(* resetFunc) (void) = 0;
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
/*void sinelon()
{
    FastLED.setBrightness(BRIGHTNESS1);
    fadeToBlackBy(leds,NUM_LEDS, 150);
    int pos = beatsin16(17, 0, NUM_LEDS-1);
    //leds[pos] = (0U, 0U, 255U);
    leds[pos] = (CRGB::Blue);
}*/
void sinelon()
{
    //FastLED.setBrightness(BRIGHTNESS1);
    fadeToBlackBy(leds,NUM_LEDS, 150);
    int pos = beatsin16(11, 0, NUM_LEDS-1);
    
    //leds[pos] = (0U, 0U, 255U);
    leds[pos] = (CRGB::Blue);
}
void setColor(int Red, int Green, int Blue)
{
  #ifdef COMMON_ANODE
    Red = 255 - RedPin;
    Green = 255 - GreenPin;
    BluePin = 255 - BluePin;
  #endif
  analogWrite(REDLIGHT_PIN, Red);
  analogWrite(GRRENLIGHT_PIN, Green);
  analogWrite(BLUELIGHT_PIN, Blue);
  
}
int MOTIONPIR(int PIRStateControl)
{
  PIRState = LOW;
  NextionReadFromArduEEPROM = EEPROM.read(0);
    if (NextionReadFromArduEEPROM == 1)
    {
      //Serial.println("PIR is ON");
      PIRState = digitalRead(PIR);
    }else if (NextionReadFromArduEEPROM == 2)
    {
      //Serial.println("PIR is Off");
      PIRState = LOW;
    }
  /*if(PIRON)
  {
    PIRState = digitalRead(PIR);
  }else
  {
    PIRState = LOW;
  }*/
  if (PIRState == LOW)
  {
    //Serial.println("No Motion!");
    PIRStateControl = 0;
    CountPir++;
    //return PIRStateControl;
  }
  else if (PIRState == HIGH)
  {
    Serial.println("Motion Detected!");
    PIRStateControl = 1;
    //return PIRStateControl;
  }
  return PIRStateControl;
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
    
    //setColor(Off, Off, Off);
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
    if (DOOROPENEDENABLE == 1 && BuzzerON)
    {
      tone(Buzzer, BuzzerFrequency1);
    }
    //setColor(On, On, Off);
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
  //setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  WS2812BGreen();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  if(BuzzerON)
  {
    Serial.print("Sipolok");
    tone(Buzzer, BuzzerFrequency1);
  }
  delay(150);
  //setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  if(BuzzerON)
  {
    noTone(Buzzer);
  }
  delay(150);
  //setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  WS2812BGreen();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  if(BuzzerON)
  {
    tone(Buzzer, BuzzerFrequency1);
  }
  delay(1000);

  //setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  if(BuzzerON)
  {
    noTone(Buzzer);
  }
  delay(150);
  //Blue
  //setColor(Off, Off, On);
  //Release Motion RGB RING Effect (Fades)
  PIRStat2 = 0;
}
void LedBuzzerAccessDenied()
{
  int ledx = 0;
  for (ledx = 0; ledx < 13; ledx++)
  {
    //leds[ledx] = (255, 0, 0);
    leds[ledx] = (CRGB::Red);
  }
//setColor(On, Off, Off);
WS2812BRed();
FastLED.show();
  //analogWrite(Buzzer, BuzzerOn);
  if(BuzzerON)
  {
    tone(Buzzer, BuzzerFrequency1);
  }
  delay(150);
  //setColor(Off, Off, Off);
  WS2812BBlack();
  FastLED.show();
  //analogWrite(Buzzer, BuzzerOff);
  if(BuzzerON)
  {
    noTone(Buzzer);
  }
  delay(150);
  //Blue
  //setColor(Off, Off, On);
  //Release Motion RGB RING Effect (Fades)
  PIRStat2 = 0;
}
void setup()
{
  wdt_disable();
  wdt_enable(WDTO_8S);
  //this should be only once call, than it should comment (Timeadjust)
  //TIMEAdjust();
  //Arduino Software RESET
  //pinMode(RSTARDU, OUTPUT);
  FirstTime = millis();
  byte FastkledBrightlvl;
  FastkledBrightlvl = EEPROM.read(4);
  byte FastledStatus = EEPROM.read(1);
  if(FastkledBrightlvl == 3 || FastledStatus == 2)
  {
    FastLED.setBrightness(BRIGHTNESSOFF);
  }else if(FastkledBrightlvl == 4)
  {
    FastLED.setBrightness(BRIGHTNESS4);
  }else if(FastkledBrightlvl == 5)
  {
    FastLED.setBrightness(BRIGHTNESS3);
  }else if(FastkledBrightlvl == 6)
  {
    FastLED.setBrightness(BRIGHTNESS2);
  }else if(FastkledBrightlvl == 7 || FastledStatus == 1)
  {
    FastLED.setBrightness(BRIGHTNESS1);
  }
  BuzzerPreConf = EEPROM.read(3);
  if(BuzzerPreConf == 1)
  {
    BuzzerON = true;
  }else if(BuzzerPreConf == 2)
  {
    BuzzerON = false;
  }
    
  FastLED.setMaxRefreshRate(40000, true);
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).getMaxRefreshRate();
  //FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear(leds);
  //Set PIR on or off 1 is on, 2 is off
  //EEPROM.put(0, 2);
  
  //FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, NUM_LEDS);
  pinMode(REDLIGHT_PIN, OUTPUT);
  pinMode(GRRENLIGHT_PIN, OUTPUT);
  pinMode(BLUELIGHT_PIN, OUTPUT);
  pinMode(MagneticLock, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  //analogWrite(Buzzer, BuzzerOn);
  if(BuzzerON)
  {
    tone(Buzzer, BuzzerFrequency1);
  }
  pinMode(Button, INPUT);
  pinMode(PIR, INPUT);
  pinMode(Reed, INPUT_PULLUP);
  delay(100);
  if (currentMillis < time_half_second && BuzzerON)
  {
    //analogWrite(Buzzer, BuzzerOff);
    noTone(Buzzer);
    //setColor(Off, Off, Standby);
  }
  //UART
  Serial.begin(115200);
  //Lora UART
  Serial1.begin(115200);
  Serial.println();
  Serial.println("Hello!");
  
  //nexInit();
  //NExtion UART

  Serial3.begin(115200);

  
  delay(10);

  Serial3.print("rest");
  Serial3.print("\xFF\xFF\xFF");
  //InsideEEPROM_Conf should run only once, after comment it and reupload the firmware
  //InsideEEPROM_Conf();

  NextionReadArduinoEEPROM();

  //rtc.begin();
  NextionStartTime = millis();
  //////////////////////////////////////

   if (! rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    //while (1);
  }else
  
  Serial.println("RTC Has just Started!");
  ////////////////////////////////////////////
  Serial.println("Itt");
  ///////////////////////

  TimeNow();
  //////////////////////////////
  LoRaReset();
  delay(2000);
  
  Wire.begin();
  
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
  if(myRfid.begin())
    Serial.println("RFID is ready to use!"); 
  else
    Serial.println("Could not communicate with Qwiic RFID!");
  NFCINITIALIZE ();
  //cardreading ();
  
  //EEPROMDeleteAll();
  //writeTo(chipAddress, eeprom_All_Card_Data_Bytes, 0);

  EEPROMReadOutAll();
  
  EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont);
  EEPROM.get(8, Ardueeprom_card_num_cont);
  Serial.print("Last Card data address in Arduino EEPROM is: "); Serial.println(Ardueeprom_all_Card_data_bytes_cont);
  Serial.print("Number of Card in Arduino EEPROM is: "); Serial.println(Ardueeprom_card_num_cont);

  eeprom_All_Card_Data_Bytes_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
  Serial.print("Last Card data address is: "); Serial.println(eeprom_All_Card_Data_Bytes_Cont);
  Serial.print("Number of Card is: "); Serial.println(eeprom_Card_Num_Cont);
}
void RecieveDataFromGateLoRa()
{
  //EEPROMDeleteAll();
  String CardDataNUMBERFROMLoRa = "";
  boolean CARDDBool = false;
  while(Serial1.available() > 0)
  {
    ControlData[CTD] = Serial1.read();
    if(ControlData[CTD] == 0x2A)
    {
        Serial.print("Itt");
        Serial.println(CTD);
        //Itt kapna meg, hogy hany db kartya van a db-ben. Ezt jelenleg uint8_t-ben mentett adott karakter ASCII kodja    
    }
    CTD++;
  }
  
  if(CTD != 0)
  {
    Serial.println("Data form OutSide Lora is: ");
    for (unsigned int i = 0; i < sizeof(ControlData); i ++)
    {
      if(ControlData[i] != 0)
      {
        Serial.println(ControlData[i], HEX);
      }
    }
    //Serial.println();
  }
  if(ControlData[0] == 0xC)
  {
    LedBuzzerAccessGranted();
    Serial.println("Access Granted, door has just opened!");
    Serial.println("Waiting for an ISO14443A Card ...");
  }else if (ControlData[0] == 0xB)
  {
    for (int i = 0; i < 3; i++)
    {
      LedBuzzerAccessDenied();
    }
    Serial.println("Access Denied!");
    Serial.println("Waiting for an ISO14443A Card ...");
  }else if (ControlData[0] == 255 && ControlData[1] == 254)
  {
    Serial.println("It's card data package!");
    //eeprom_All_Card_Data_Bytes_Cont = 0;
    //Send Card flag to Nextion
    Serial3.print("page11.RDDDTimeout.val=");
    Serial3.print(122);
    Serial3.print("\xFF\xFF\xFF");
    EEPROM_RGB_BLINK();
    CardfromDBToEEPROM();
    EEPROM_RGB(0, 0, 0);
    //EEPROM_TIME_OUT_F();
  }else if(ControlData[0] == 0x2A && ControlData[1] == 0x2A && ControlData[2] == 0x2D && ControlData[4] == 0x2A)
  {
    //how much card in DB
    EEPROM_RGB_BLINK_BOOL = true;
    EEPROM_RGB_BLINK();
    //1st clear eeprom
    EEPROM_Vansiher();
    //Store Number of Cards from DB
    //ControlData[3] = ControlData[3] - 6;
    
    Serial.print("Number of cards from DB are: "); Serial.println(ControlData[3], DEC);
    //Store Number of cards in EEPROM
    unsigned int IntCardNum = (int)ControlData[3];
    
    Serial.print("Unit8_t converted to number of card value to int: "); Serial.println(IntCardNum);
    EEPROM.put(8, Ardueeprom_card_num_cont);  
    writeTo(chipAddress, eeprom_Card_Num,  ControlData[3]);
    //Release Last Card data address
    EEPROM.put(7, 0);
    writeTo(chipAddress, eeprom_All_Card_Data_Bytes, 0);
     
    CardNumberStore = 0;
    Serial.print("Last Card address data in eeprom: "); Serial.println(readFrom(chipAddress, eeprom_All_Card_Data_Bytes));
    Serial.print("Last Card address data in Arduino eeprom: "); Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
    Serial.print("Number of cards in eeprom: "); Serial.println(readFrom(chipAddress, eeprom_Card_Num));
    Serial.print("Number of cards in Arduino eeprom: "); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
    //Serial.print("Read from eeprom: "); Serial.println(readFrom(chipAddress, eeprom_All_Card_Data_Bytes));
    
    
  }else if(ControlData[0] == 63 && ControlData[1] == 63 && ControlData[2] == 64)
  {
    EEPROM_RGB_FINISH();
    EEPROM_RGB(0, 255, 0);
    

    //writeTo(chipAddress, eeprom_All_Card_Data_Bytes, 0);
  }else if(ControlData[0] == 113 && ControlData[1] == 113 && ControlData[2] == 239)
  {
    Serial.println("Card Data Download finished");
    Serial3.print("page11.RDDDTimeout.val=");
    Serial3.print(599);
    Serial3.print("\xFF\xFF\xFF");
    SendEEPROM_conf_to_Nextion();
  }else if(ControlData[0] == 113 && ControlData[1] == 113 && ControlData[2] == 240)
  {
    Serial.println("Connection established");
    Serial3.print("page11.RDDDTimeout.val=");
    Serial3.print(122);
    Serial3.print("\xFF\xFF\xFF");
    Serial.println("Connection data has sent to Nextion");
  }else if(ControlData[0] == 113 && ControlData[1] == 113 && ControlData[2] == 241)
  {
    Serial.println("The Database is empty!");
    Serial3.print("page11.RDDDTimeout.val=");
    Serial3.print(600);
    Serial3.print("\xFF\xFF\xFF");
  }
  //EEPROM_RGB_FINISH();
 //Release Data
  for(unsigned int i = 0; i < sizeof(ControlData); i++)
  {
    ControlData[i] = 0;
  }
  CTD = 0;
  
  
}
void CardfromDBToEEPROM()
{
  EEPROM_RGB_BLINK();
  //Összes kártya byte
  eeprom_All_Card_Data_Bytes_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont);
  //Összes kártya száma
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
  EEPROM.get(8, Ardueeprom_card_num_cont);
  Serial.print("Last Card data address is: "); Serial.println(eeprom_All_Card_Data_Bytes_Cont);
  Serial.print("Number of Card is from EEPROM: "); Serial.println(eeprom_Card_Num_Cont);

  Serial.print("Last Card data address In Arduiono EEPROM is: "); Serial.println(Ardueeprom_all_Card_data_bytes_cont);
  Serial.print("Number of Card is from Arduino EEPROM: "); Serial.println(Ardueeprom_card_num_cont);
  //Delete all Card data
  while(Cardi < sizeof(ControlData))
  {
    EEPROM_RGB_BLINK();    
    writeTo(chipAddress, eeprom_All_Card_Data_Bytes_Cont, ControlData[Cardi]);
    Serial.print(eeprom_All_Card_Data_Bytes_Cont);
    Serial.print(" Actual UID byte is: "); Serial.println(readFrom(chipAddress, eeprom_All_Card_Data_Bytes_Cont), HEX);
    EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont);
    Serial.print(" Actual UID byte in Arduino EEPROM is: "); Serial.println(Ardueeprom_all_Card_data_bytes_cont, HEX);
    Cardi++;
    eeprom_All_Card_Data_Bytes_Cont++;
  }

  //eeprom_Card_Num_Cont++;

  //eeprom_Card_Num_Cont = Cardi-2;
  //CardNumberStore += eeprom_Card_Num_Cont;
  CardNumberStore++;
  Serial.print("eeprom Card Num Cont is: "); Serial.println(CardNumberStore);
  Cardi = 2;
  EEPROM.put(8, CardNumberStore);
  writeTo(chipAddress, eeprom_Card_Num, CardNumberStore);
  Serial.println("Ardu EEPROM Cards is ON.");
  EEPROM.write(6, 1);
  EEPROM.put(7, eeprom_All_Card_Data_Bytes_Cont);
  writeTo(chipAddress, eeprom_All_Card_Data_Bytes, eeprom_All_Card_Data_Bytes_Cont);
  EEPROM_RGB_BLINK();
  unsigned long i = 0;
  Serial.print("Actual cards in eeprom: ");
  Serial.println(readFrom(chipAddress, eeprom_Card_Num));
  
  Serial.print("Actual cards in Arduino eeprom: ");
  Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));

  Serial.print("Actual last card add is eeprom: ");
  Serial.println(readFrom(chipAddress, eeprom_All_Card_Data_Bytes));

  Serial.print("Actual last card add is Arduino eeprom: ");
  Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
  //EEPROMReadOutAll();
  /*while (true)
  {
    Serial.println(readFrom(chipAddress, i));
    i++;
    delay(250);
  }*/
}
void HC05JDY30FUNCTION()
{
  if(HC05JDY30.available() > 0)
  { // Checks whether data is comming from the serial port
    state = HC05JDY30.read(); // Reads the data from the serial port
    if (state == 1)
    {
      LedBuzzerAccessGranted();
      TimeStamp();
      Serial1.write(HC05Status, sizeof(HC05Status));
      Serial1.write(DS3231Time, sizeof(DS3231Time));
      Serial.println("Access Granted, door has just opened by SmartPhoneApp!");
    }
  }
  return;
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
        if(!EEPROM_RGB_BLINK_BOOL)
        {
          sinelon();
        }
      }
      ReedState = digitalRead(Reed);
      DoorTimeOut++;
      /*Serial.print(DoorTimeOut);*/
      /*CurrentTimeDoorOpenedMillis = millis();
      Serial.print("Current Millis: "); Serial.println(millis());*/
      //if (CurrentTimeDoorOpenedMillis >= WaitingOpenedDoor)

      //Serial.println(DoorTimeOut);

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
///////////////////////////////////
        //return ReturnDoorTimeOut = 1;
////////////////////////////////////////
        /*if (ReedState == LOW)
        {
          DoorTimeOut = 0;
          Serial.print(DoorTimeOut); Serial.println("Released");
          setColor(Off, Off, Standby);
        }*/
        return ReturnDoorTimeOut = 1;
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
/////////////////////////////////////
      //return ReturnDoorTimeOut = 0;
///////////////////////////////////

    }
  /////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////
  }else if (DOOROPENEDENABLE == 0)
  {
  /////////////////////////////////////////////////////////////////////
  DoorTimeOut = 0;
  /////////////////////////////////////////////////////////////////////
  }else
  {
    ReedState = digitalRead(Reed);
    if (ReedState == HIGH)
    {
      if (DoorOpenStatusForWS2812B != 1)
      {
        if(!EEPROM_RGB_BLINK_BOOL)
          {
            sinelon();
          }
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
//////////////////////
        //return ReturnDoorTimeOut = 1;
/////////////////////
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
      //setColor(Off, Off, Standby);
      //sinelon();
      //analogWrite(Buzzer, BuzzerOff);

      //noTone(Buzzer);

      /*Serial.print(DoorTimeOut);
      Serial.println(" Locked");*/
////////////////////
      //return ReturnDoorTimeOut = 0;
//////////////////
    }
  }
  return ReturnDoorTimeOut;
  /////////////////////////////////////////////////////////////////////
}
void loop()
{
  wdt_reset();
  if(BuzzerON)
  {
    noTone(Buzzer);
  }
  //FastLED.delay(1000/FRAMES_PER_SECOND);
  //EVERY_N_MILLISECONDS(100);
  //**Serial.println(CountPir);
  currentMillis = millis();
  NextionCurrentTime = millis();
  
  ReadFromNextion();
  SendTimeToNextion();
  
  
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
        
        Serial.println("Itt2!");
        //setColor(Off, Off, On);
        WS2812BBlue();
        WS2812BTrunOff = 1;
        PIRCount++;
        //Serial.print("PIRCount: "); Serial.println(PIRCount);
        if (PIRCount >= PowerOnTime)
        {
          if(PIRON)
          {
            PIRState = digitalRead(PIR);
          }
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
        if(!EEPROM_RGB_BLINK_BOOL)
        {
        sinelon();
        }
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
      ACCOffline();
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
  tag = "";
    tag = myRfid.getTag();   
      if (tag.toInt() < 0)
      {
        BuzzerCarding();
        Serial.print("Original RFID tag: "); Serial.println(tag);
        //Serial.print("Original tag length: "); Serial.println(tag.length());
        for(int i = 0; i < 10; i++)
        {
          WS2812BBlue();
        }
      }
      if(tag.toInt() < 0)
      {
        Serial.println("it's RFID tag");
        unsigned int TagLength = tag.length();
        //Serial.print("int taglength is: "); Serial.println(TagLength);
        while (TagLength < 16)
        {
          //Serial.print("int2 taglength is: "); Serial.println(TagLength);
          for (iRFID = 0; iRFID < TagLength; iRFID++)
          {
            DataString = tag.substring(c, b);
            //Serial.print(DataString + "\n");
            IntegerDataStringArray[iRFID] = DataString.toInt();
            b++;
            c++;
          }
          iRFID++;
          //Serial.print("iRFID is: "); Serial.println(iRFID);
          while (TagLength < 16)
          {
            IntegerDataStringArray[iRFID] = RFIDID;
            TagLength++;
            iRFID++;
            RFIDID--;
          }
        }
        //Serial.print("Final RFID TAG Lenght: "); Serial.println(TagLength);
        if (TagLength == 16)
        {
          for (unsigned int u = 0; u < TagLength; u++)
          {
            if (u <= 3)
            {
              uid[0] += IntegerDataStringArray[u];
              //Serial.print("1st data structure is: "); Serial.println(uid[0]);
            }
            if (u <= 7 && u > 3)
            {
              uid[1] += IntegerDataStringArray[u];
              //Serial.print("2nd data structure is: "); Serial.println(uid[1]);
            }
            if (u <= 11 && u > 7)
            {
              uid[2] += IntegerDataStringArray[u];
              //Serial.print("IntegerDataStringArray is: "); Serial.println(IntegerDataStringArray[u]);
              //Serial.print("3th data structure is: "); Serial.println(uid[2]);
            }
            if (u <= 15 && u > 11)
            {
              uid[3] += IntegerDataStringArray[u];
              //Serial.print("4th data structure is: "); Serial.println(uid[3]);
            }
          }
        }
        char uidChar[7] = "";
        
        //Release variables
        b = 1;c = 0;iRFID = 0;RFIDID = 14;
        Serial.print("TAG ID: ");Serial.print("Normal: ");
        for (int p = 0; p < 4; p++)
        {
        //Serial.print("0x"); 
        Serial.print(uid[p]);
        //Serial.print(" ");
        uidLength++;
        }
        Serial.println();
        sprintf(uidChar, "%X%X%X%X", uid[0], uid[1], uid[2], uid[3]);
        String CardData = uidChar;
        Serial.print("Modified: "); Serial.println(CardData);
        for (int p = 0; p < 4; p++)
        {
        //Serial.print("0x"); Serial.print(uid[p]);Serial.print(" ");
        uidLength++;
        }
        Serial.println();
        uidLengthplus = uidLength+2;
        if (uid[0] > 0)
        {
          //Serial.println("1");
          if (uid[1] > 0)
          {
            //Serial.println("2");
            if (uid[2] > 0)
            {
              //Serial.println("3");
              if (uid[3] > 0)
              {
                //Serial.println("4");
                //Serial.print("UidLengthplus is: "); Serial.println(uidLengthplus);
                for(int h = 2; h < uidLengthplus; h++)
                {
                  uidDataPackage[h] = uid[uidBytesCount];
                  //It's for to know if it's from RFID or from NFC Reader
                  uidDataPackage[1] = 155;
                  uidBytesCount++;
                  //Serial.println("ITTENVAGYOK");
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
      }
      //*****************************************************************************************
    // configure board to read RFID tags
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    nfc.SAMConfig();

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);//, timeout = 50);
    success = nfc.inListPassiveTarget();

    //It's for NFC Carding detection
    uidDataPackage[0] = 255;
    uidDataPackage[1] = 255;
    // Display some basic information about the card
    if (uid[1] > 0)
      {
        BuzzerCarding();
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        Serial.println();
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
          /*else
          {
            WS2812BBlue();
          }
          
            //WS2812BBlue();*/
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
          else
            WS2812BBlue();
        }*/
        //Serial.print("UIDLength: "); Serial.println(uidLength);
        if(uidLength == 4)
        {
          uidLengthplus = uidLength+2;
        }
        //else if (uidLength == 7){uidLengthplus = uidLength+2;}
        else if (uidLength >= 5 || uidLength < 4)
        {
          Serial.print("The "); Serial.print(uidLength);
          Serial.println(" Bytes UID is not supported yet!");
          NoSuppBuzzer();
          for (unsigned int i = 0; i < sizeof(uid); i++)
          {
            uid[i] = 0;
          }
          uidLength = 0;
          ret = false;
          return ret;
        }
        
        for(int h = 2; h < uidLengthplus; h++)
        {
          Serial.print("UIDBYTESCOUNT IS: "); Serial.print(uidBytesCount);
          uidDataPackage[h] = uid[uidBytesCount];
          uidBytesCount++;

          Serial.print(" "); Serial.print(uidDataPackage[h], HEX);Serial.println(" ");
        }
        Serial.println();
        
        Serial.println();

        uidBytesCount = 0;
        ret = true;
      }
    else if (uid[1] == 0)
    {
      ret = false;
      uidBytesCount = 0;
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
    Serial.print("DataPackage is: ");
    for(int hu = 0; hu < sizeof(uidDataPackage); hu++)
    {
      Serial.print(uidDataPackage[hu], HEX);Serial.print(" ");
    }
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
    Serial.print("The size of UID is: "); Serial.println(sizeof(uid));
    for (unsigned int i = 0; i < sizeof(uid); i++)
    {
      uid[i] = 0;
      /*Serial.print(i);
      Serial.print(" ");
      Serial.println(uid[i]);*/
    }
    uidLength = 0;
    Serial.println("UID, and UIDLength HAS Been Cleared!");
    for (unsigned int ib = 2; iRFID < sizeof(uidDataPackage); iRFID++)
    {
      uidDataPackage[ib] = 0;
      //Serial.print("0xx"); Serial.print(uidDataPackage[ib]);Serial.print(" ");
    }
    uidLengthplus = 0;
    
    Serial.println("UidDataPackage HAS Been Cleared!");
    for (unsigned int i = 0; i < sizeof(uid); i++)
    {
      //int UIDDelete = uid[i];
      Serial.print("0x");
      Serial.print(uid[i], HEX);
    }
    Serial.print("\n");
    for (unsigned int ib = 0; iRFID < sizeof(uidDataPackage); iRFID++)
    {
      //int UIDDelete = uidDataPackage[ib];
      Serial.print("0x");
      Serial.print(uidDataPackage[ib], HEX);
    }
    Serial.print("\n");
    Serial.println();
    
    //Serial.println("Waiting for a card...");
    
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
        ControlData[0] = 10;
        LedBuzzerAccessGranted();
        Serial.println("Access Granted, door has just opened by Buttonat: ");
        TimeStamp();

        Serial1.write(ButtonDataPackage, sizeof(ButtonDataPackage));
        Serial1.write(DS3231Time, sizeof(DS3231Time));
        Serial.println("Waiting for an ISO14443A Card ...");
        ButtonValToDoorControl = 0;
        Serial.print("Button datapck is: ");
        for (unsigned int i = 0; i < sizeof(ButtonDataPackage); i++)
        {
          Serial.print(ButtonDataPackage[i], HEX);
        }
        Serial.println();
    }
  }
}
void TIMEAdjust()
{
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(11));
  //rtc.adjust((DateTime(2020, 7, 22, 18, 14, 0)) + TimeSpan(11));
  
  now = rtc.now();
}
void TimeNow()
{
  now = rtc.now();
    if (rtc.lostPower())
    {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(11));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //now = rtc.now().unixtime();
    now = rtc.now();
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
    for(int i = 0; i < sizeof(DS3231Time); i++)
    {
      DS3231Time[i] = 0;
    }
    now = rtc.now();
    //Serial.println("Time Stamp: ");
    DS3231Time[0] = now.year()-2000; DS3231Time[1] = now.month(); DS3231Time[2] = now.day();
    DS3231Time[3] = now.dayOfTheWeek(); DS3231Time[4] = now.hour(); DS3231Time[5] = now.minute(); DS3231Time[6] = now.second();
    //Year
    Serial.print("20"); Serial.print(DS3231Time[0]);
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
  if(BuzzerON)
  {
    tone(Buzzer, BuzzerFrequency1);
    delay(100);
    noTone(Buzzer);
    delay(100);
  }
}
void NoSuppBuzzer()
{
  /*tone(Buzzer, BuzzerFrequency2);
  delay(100);
  noTone(Buzzer);
  delay(100);*/
  for (int t = 0; t < 4; t++)
  {
  WS2812BRed();
  FastLED.show();
  if(BuzzerON)
  {
    tone(Buzzer, BuzzerLowFreq1);
  }
  delay(100);
  WS2812BBlack();
  FastLED.show();
  if(BuzzerON)
  {
    noTone(Buzzer);
  }
  delay(100);
  }
}
void ReadFromNextion()
{
    NextionDataRec = false;
    while(Serial3.available() > 0)
    {
        NextionBuffer[NextionI] = Serial3.read();
        Serial.print("Data "); Serial.print(NextionI); Serial.print(" is "); Serial.println(NextionBuffer[NextionI], HEX);
        NextionI++;
        if(NextionI > 6)
        {

            NextionI = 0;
        }
        NextionDataRec = true;

    }
    if(NextionDataRec)
    {
        //Serial.print("Data received "); Serial.print(NextionI); Serial.println(".");
        /*0x65 0x00 0x01 0x01 0xFF 0xFF 0xFF
          Returned when Touch occurs and component’s
          corresponding Send Component ID is checked
          in the users HMI design.
          0x65 is byte ID, 0x00 is page number, 0x01 is component ID,
          0x01 is event (0x01 Press and 0x00 Release)
        */
        //Page0 Success
        if(NextionBuffer[0] == 104)
        {
            BuzzNextionSuccess();
            Serial.println("Success");
        //Page0 Denied
        }else if(NextionBuffer[0] == 105)
        {
            BuzzNextionDenied();
            Serial.println("Denied");
            Serial1.write(InvaildSingInGateDP, sizeof(InvaildSingInGateDP));
            Serial1.write(DS3231Time, sizeof(DS3231Time));
        //Page0 Touch Release
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x0) &&
          (NextionBuffer[2] == 0x3 || NextionBuffer[2] == 0x4 || NextionBuffer[2] == 0x5 || NextionBuffer[2] == 0x6
          || NextionBuffer[2] == 0x7 || NextionBuffer[2] == 0x8 || NextionBuffer[2] == 0x9 || NextionBuffer[2] == 0xA
          || NextionBuffer[2] == 0xB || NextionBuffer[2] == 0xC || NextionBuffer[2] == 0xD || NextionBuffer[2] == 0xE
          || NextionBuffer[2] == 0xF || NextionBuffer[2] == 0x10 || NextionBuffer[2] == 0x11 || NextionBuffer[2] == 0x12) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
            NextionBuzzer();
        //Page0 Countdown
        }else if
          (NextionBuffer[0] == 2 && NextionBuffer[1] == 2)
        {
            NextionPassCodeTimeOut();
        //Page1 Next
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x1) &&
          (NextionBuffer[2] == 0x6 || NextionBuffer[2] == 0x1C) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {

            NextionBuzzer();       
        //Page4 Quit
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x04) &&
          (NextionBuffer[2] == 0x13) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzerQuit();
          //Last Carding Back Buzzer
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x06) &&
          (NextionBuffer[2] == 0x02) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
          //EEPROM Delete Buzzer no
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x09) &&
          (NextionBuffer[2] == 0x03) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
          //Set RTC Buzzer
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x07) &&
          (NextionBuffer[2] == 0x02 || NextionBuffer[2] == 0x03 || NextionBuffer[2] == 0x04 || NextionBuffer[2] == 0x08 || NextionBuffer[2] == 0x09|| NextionBuffer[2] == 0xA || NextionBuffer[2] == 0x05 || NextionBuffer[2] == 0x06 || NextionBuffer[2] == 0x07 || NextionBuffer[2] == 0xB || NextionBuffer[2] == 0xC || NextionBuffer[2] == 0xD || NextionBuffer[2] == 0xE) &&
          (NextionBuffer[3] == 0x0 && 
          NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x04) && 
          (NextionBuffer[2] == 0xE || NextionBuffer[2] == 0x02 || NextionBuffer[2] == 0xF || NextionBuffer[2] == 0x10
          || NextionBuffer[2] == 0x11 || NextionBuffer[2] == 0x12 || NextionBuffer[2] == 0x13 || NextionBuffer[2] == 0x14
          || NextionBuffer[2] == 0x15) &&
          (NextionBuffer[3] == 0x0 || NextionBuffer[3] == 0x1) && 
          (NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
        //No Connection button pushed
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0xB) && 
          (NextionBuffer[2] == 0xA) &&
          (NextionBuffer[3] == 0x0) && 
          (NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
        //Page 7 Set RTC Back button
        }else if(
          (NextionBuffer[0] == 0x65 && NextionBuffer[1] == 0x7) && 
          (NextionBuffer[2] == 0xE) &&
          (NextionBuffer[3] == 0x0) && 
          (NextionBuffer[4] == 0xFF && NextionBuffer[5] == 0xFF && NextionBuffer[6] == 0xFF))
        {
          NextionBuzzer();
        //Page 4 back to page 2 or go to Retry Door Data Download page
        }else if(NextionBuffer[0] == 0x58)
        {
          NextionBuzzer();
          //Retry Card Data Download
        }else if(NextionBuffer[0] == 0x22 && NextionBuffer[1] == 0x43 && NextionBuffer[2] == 0x14)
        {
          Serial.println("Retry Door data download request from server");
          TimeStamp();
          Serial1.write(RetryCardDataDownloadDataPackage, sizeof(RetryCardDataDownloadDataPackage));
          Serial1.write(DS3231Time, sizeof(DS3231Time));
          //Page 2 enter
        }
        else if(NextionBuffer[0] == 103)
        {
          Serial.println("Page 1");
          SendEEPROM_conf_to_Nextion();
          //Master User Entered
        }else if(NextionBuffer[0] == 0x10 && NextionBuffer[1] == 0x11 && NextionBuffer[2] == 0x22)
        {
          Serial.println("Master User Enter.");
          Serial1.write(MasterUserDP, sizeof(MasterUserDP));
          Serial1.write(DS3231Time, sizeof(DS3231Time));
          //Master User Quit
        }else if(NextionBuffer[0] == 0xA0 && NextionBuffer[1] == 0xB0 && NextionBuffer[2] == 0xC0)
        {
          Serial.println("Master User Quit.");
          Serial1.write(MasterUserDPQuit, sizeof(MasterUserDPQuit));
          Serial1.write(DS3231Time, sizeof(DS3231Time));
          NextionBuzzerQuit();
          //Page 4 enter
        }
        else if(NextionBuffer[0] == 13)
        {
          Serial.println("Page 4");
                  
          SendEEPROM_conf_to_Nextion();
        //LED ON
        }else if(NextionBuffer[0] == 99)
        {
          Serial.println("Led On");
          EEPROM.write(1, 1);
          EEPROM.write(4, 7);
          FastLED.setBrightness(BRIGHTNESS1);
          SendEEPROM_conf_to_Nextion();
        //LED Off
        }else if(NextionBuffer[0] == 98)
        {
          Serial.println("Led Off");
          EEPROM.write(1, 2);
          EEPROM.write(4, 3);
          FastLED.setBrightness(BRIGHTNESSOFF);
          SendEEPROM_conf_to_Nextion();
        //REED ON
        }else if(NextionBuffer[0] == 97)
        {
          Serial.println("Reed On");
          EEPROM.write(2, 1);
          DOOROPENEDENABLE = 1;
          SendEEPROM_conf_to_Nextion();
        //REED Off
        }else if(NextionBuffer[0] == 96)
        {
          Serial.println("Reed Off");
          EEPROM.write(2, 2);
          DOOROPENEDENABLE = 0;
          SendEEPROM_conf_to_Nextion();
        //Buzzer On
        }else if(NextionBuffer[0] == 95)
        {
          Serial.println("Buzzer On");
          EEPROM.write(3, 1);
          BuzzerON = true;
          SendEEPROM_conf_to_Nextion();
        //Buzzer Off
        }else if(NextionBuffer[0] == 94)
        {
          Serial.println("Buzzer Off");
          BuzzerON = false;
          EEPROM.write(3, 2);
          SendEEPROM_conf_to_Nextion();
        //PIR ON
        }else if(NextionBuffer[0] == 91)
        {
          PIRON = true;
          Serial.println("Pir On");
          EEPROM.write(0, 1);
          SendEEPROM_conf_to_Nextion();
        //PIR Off
        }else if(NextionBuffer[0] == 90)
        {
          Serial.println("Pir Off");
          EEPROM.write(0, 2);
          PIRON = false;
          SendEEPROM_conf_to_Nextion();
        }else if(NextionBuffer[0] == 93)
        {
          Serial.println("Reseted");
          resetFunc();
        //Security door open
        }else if(NextionBuffer[0] == 92)
        {
          LedBuzzerAccessGranted();
          Serial.println("Door Opened by security reason at: ");
          TimeStamp();
          //Serial1.write(NextionOutSideDataPackage, sizeof(NextionOutSideDataPackage));
          Serial1.write(SecurityDataPackage, sizeof(SecurityDataPackage));
          Serial1.write(DS3231Time, sizeof(DS3231Time));
          Serial.println("Waiting for a Card...");
        //0% Led intensity
        }else if(NextionBuffer[0] == 3)
        {
          Serial.println("Led intensity 0%");
          EEPROM.write(4, 3);
          EEPROM.write(1, 2);
          FastLED.setBrightness(BRIGHTNESSOFF);
          SendEEPROM_conf_to_Nextion();
        //25% Led intensity
        }else if(NextionBuffer[0] == 7)
        {
          Serial.println("Led intensity 100%");
          EEPROM.write(4, 7);
          EEPROM.write(1, 1);
          FastLED.setBrightness(BRIGHTNESS1);
          SendEEPROM_conf_to_Nextion();
        //50% Led intensity
        }else if(NextionBuffer[0] == 5)
        {
          Serial.println("Led intensity 50%");
          EEPROM.write(4, 5);
          EEPROM.write(1, 1);
          FastLED.setBrightness(BRIGHTNESS3);
          SendEEPROM_conf_to_Nextion();
        //75% Led intensity
        }else if(NextionBuffer[0] == 6)
        {
          Serial.println("Led intensity 75%");
          EEPROM.write(4, 6);
          EEPROM.write(1, 1);
          FastLED.setBrightness(BRIGHTNESS2);
          SendEEPROM_conf_to_Nextion();
        //100% Led intensity
        }else if(NextionBuffer[0] == 4)
        {
          Serial.println("Led intensity 25%");
          EEPROM.write(4, 4);
          EEPROM.write(1, 1);
          FastLED.setBrightness(BRIGHTNESS4);
          SendEEPROM_conf_to_Nextion();
        //Nextion Display Backlight level is 25%
        }else if(NextionBuffer[0] == 8)
        {
          Serial.print("Nextion Display Blacklight percent is: 25% .");
          EEPROM.write(5, 25);
          SendEEPROM_conf_to_Nextion();
        //Nextion Display Backlight level is 50%
        }else if(NextionBuffer[0] == 9)
        {
          Serial.print("Nextion Display Blacklight percent is: 50% .");
          EEPROM.write(5, 50);
          SendEEPROM_conf_to_Nextion();
        //Nextion Display Backlight level is 75%
        }else if(NextionBuffer[0] == 10)
        {
          Serial.print("Nextion Display Blacklight percent is: 75% .");
          EEPROM.write(5, 75);
          SendEEPROM_conf_to_Nextion();
        //Nextion Display Backlight level is 100%
        }else if(NextionBuffer[0] == 11)
        {
          Serial.print("Nextion Display Blacklight percent is: 100% .");
          EEPROM.write(5, 100);
          SendEEPROM_conf_to_Nextion();
        //Nextion EEPROM delete pressed
        }else if(NextionBuffer[0] == 12)
        {
          Serial.println("EEPROM Delete in progress...");
          eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
          Serial.println("Current number of Cards before deleting them: "); Serial.println(eeprom_Card_Num_Cont);
          EEPROM.write(6, 0);
          EEPROM.get(8, Ardueeprom_card_num_cont);
          Serial.println("Current number of Cards before deleting them in Arduino EEPROM: "); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
          if(eeprom_Card_Num_Cont != 0 )
          {
            Serial.println("Card deleting started...");
            EEPROM_Vansiher();
          }else
          {
            Serial.println("No cards stored in EEPROM.");
            SendEEPROM_conf_to_Nextion();
          }
          //Send Current Time to Nextion RTC set page   
        }else if(NextionBuffer[0] == 0xF0 &&NextionBuffer[1] == 0xD5 && NextionBuffer[2] == 0xB4)
        {
          Serial.println("Set RTC mode active.");
          RTCAdjustNextion(); 
          //NextionBufferClear 
        }else if(NextionBuffer[0] == 0xA4 &&NextionBuffer[1] == 0xB4 && NextionBuffer[2] == 0xC5)
        {
          Serial.println("Nextion Time Data Package!");
          boolean RTCGet = true;
          DateTime RTCFNXT;
          String RTCDATA = "";
          uint16_t Year;
          uint8_t Month;
          uint8_t Day;
          uint8_t Hour;
          uint8_t Minute;
          uint8_t Second;
          do
          {
            RTCDATA = Serial3.readStringUntil('\r');
            if(RTCDATA != "")
            {
              RTCGet = false;
            }  
          }while (RTCGet);
          Serial.print("Get RTC Year Data: ");Serial.println(RTCDATA);
          unsigned int RTCDataLen = RTCDATA.length();
          Serial.print("Size of RTC Data is: "); Serial.println(RTCDataLen);
          char RTCD[20];
          char buf[sizeof(RTCD)];
          RTCDATA.toCharArray(buf, sizeof(buf));
          char * pch;
          String CharToStr = "";
          pch = strtok(buf, "-");
          /*Serial.print("Year: "); Serial.print(pch[0]); Serial.print(pch[1]); Serial.print(pch[2]); Serial.print(pch[3]); Serial.println(" ");
          Serial.print("Month: "); Serial.print(pch[5]); Serial.print(pch[6]); Serial.println(" ");
          Serial.print("Day: "); Serial.print(pch[8]); Serial.print(pch[9]); Serial.println(" ");
          Serial.print("Hour: "); Serial.print(pch[11]); Serial.print(pch[12]); Serial.println(" ");
          Serial.print("Minute: "); Serial.print(pch[14]); Serial.print(pch[15]); Serial.println(" ");
          Serial.print("Second: "); Serial.print(pch[17]); Serial.println(pch[18]); Serial.println(" ");*/
          CharToStr += pch[0];
          CharToStr += pch[1];
          CharToStr += pch[2];
          CharToStr += pch[3];
          Serial.print("Year Char to Int is: ");Serial.println(CharToStr);
          Year = CharToStr.toInt();
          Serial.print("Year is: "); Serial.println(Year);

          CharToStr = "";
          CharToStr += pch[5];
          CharToStr += pch[6];
          Serial.print("Month Char to Int is: ");Serial.println(CharToStr);
          Month = CharToStr.toInt();
          Serial.print("Month is: "); Serial.println(Month);

          CharToStr = "";
          CharToStr += pch[8];
          CharToStr += pch[9];
          Serial.print("Day Char to Int is: ");Serial.println(CharToStr);
          Day = CharToStr.toInt();
          Serial.print("Month is: "); Serial.println(Day);

          CharToStr = "";
          CharToStr += pch[11];
          CharToStr += pch[12];
          Serial.print("Hour Char to Int is: ");Serial.println(CharToStr);
          Hour = CharToStr.toInt();
          Serial.print("Hour is: "); Serial.println(Hour);

          CharToStr = "";
          CharToStr += pch[14];
          CharToStr += pch[15];
          Serial.print("Minute Char to Int is: ");Serial.println(CharToStr);
          Minute = CharToStr.toInt();
          Serial.print("Minute is: "); Serial.println(Minute);

          CharToStr = "";
          CharToStr += pch[17];
          CharToStr += pch[18];
          Serial.print("Second Char to Int is: ");Serial.println(CharToStr);
          Second = CharToStr.toInt();
          Serial.print("Second is: "); Serial.println(Second);
          //Input: 2018-12-31-00-11-21-
                
          rtc.adjust(DateTime(Year, Month, Day, Hour, Minute, Second) + TimeSpan(2));
          //NextionBufferClear 
        }else
        {
            for (unsigned int i = 0; i < sizeof(NextionI); i++)
            {
            NextionBuffer[NextionI] = 0;
            
            }
            NextionI = 0;
        }
        NextionDataRec = false;
    }
    //Serial.println("Ide is eljutok!");
      for (unsigned int i = 0; i < sizeof(NextionI); i++)
      {
          NextionBuffer[NextionI] = 0;
          
      }
      NextionI = 0;
      NextionDataRec = false;
}

void BuzzNextionSuccess()
{
    for(int i = 0; i < 3; i++)
    {
        tone(NextBuzzer, 2000);
        delay(70);
        noTone(NextBuzzer);
        delay(70);
    }
}
void BuzzNextionDenied()
{
        tone(NextBuzzer, 2000);
        delay(100);
        noTone(NextBuzzer);
        delay(100);
        tone(NextBuzzer, 500);
        delay(500);
        noTone(NextBuzzer);
        delay(150);
}
void NextionBuzzer()
{
        tone(NextBuzzer, 2000);
        delay(25);
        noTone(NextBuzzer);
        delay(25);
}
void NextionPassCodeTimeOut()
{
    tone(NextBuzzer, 2000);
    delay(25);
    noTone(NextBuzzer);
    delay(25);
}
void NextionBuzzerQuit()
{
    for(int i = 0; i < 3; i++)
    {
        tone(NextBuzzer, 500);
        delay(70);
        noTone(NextBuzzer);
        delay(70);
    }
}
void TimeToNextion()
{
    
    now = rtc.now();
    //now = rtc.now();
    //Serial.println("Time Stamp: ");
    DS3231Time[0] = now.year()-2000; DS3231Time[1] = now.month(); DS3231Time[2] = now.day();
    DS3231Time[3] = now.dayOfTheWeek(); DS3231Time[4] = now.hour(); DS3231Time[5] = now.minute(); DS3231Time[6] = now.second();
}
void SendTimeToNextion()
{
    if(NextionCurrentTime - NextionStartTime >= NextionTimeInterval)
    {
        NextionStartTime = NextionCurrentTime;
        TimeToNextion();
        Serial3.print("n0.val=");
        //Serial3.print(2000);
        Serial3.print(DS3231Time[0]+2000);
        Serial3.print("\xFF\xFF\xFF");
        //Serial.println(DS3231Time[0]+2000);
        Serial3.print("n1.val=");
        Serial3.print(DS3231Time[1]);
        Serial3.print("\xFF\xFF\xFF");

        Serial3.print("n2.val=");
        Serial3.print(DS3231Time[2]);
        Serial3.print("\xFF\xFF\xFF");

        Serial3.print("n3.val=");
        Serial3.print(DS3231Time[4]);
        Serial3.print("\xFF\xFF\xFF");

        Serial3.print("n4.val=");
        Serial3.print(DS3231Time[5]);
        Serial3.print("\xFF\xFF\xFF");

        Serial3.print("n5.val=");
        Serial3.print(DS3231Time[6]);
        //Serial.println(DS3231Time[6]);
        Serial3.print("\xFF\xFF\xFF");
    }
}
void NextionWriteArduinoEEPROM()
{
    NextionReadFromArduEEPROM = EEPROM.read(0);
    if (NextionReadFromArduEEPROM == 1)
    {
      Serial.println("Currently PIR is On");
      EEPROM.put(0, 2);
      Serial.println("PIR is Off");
    }else if (NextionReadFromArduEEPROM == 2)
    {
      Serial.println("Currently PIR is Off");
      EEPROM.put(0, 1);
      Serial.println("PIR is On");
    } 
}
void NextionReadArduinoEEPROM()
{
  //PIR data is EEPROM address 0
  //LED data is EEPROM address 1
  //REED data is EEPROM address 2
  //BUZZER data is EEPROM address 3
  //Led Intensity is EEPROM address 4
  //Nextion Display Backlight is EEPROM address 5
  //Are there any cards in eeprom is EEPROM address 6
  //Actual Card data byte is in EEPROM address 7
  //Actual number of Cards is in EEPROM address 8
  byte NextPIR_val, NextLED_val, NextREED_val, NextBUZZER_val, NextLedIntensity_val, NextDispBlk, NextNumCrds = 0;

  //PIR status read from EEPROM
  NextPIR_val = EEPROM.read(0);  
  if (NextPIR_val == 1)
  {
    Serial.println("PIR is ON");
  }else if (NextPIR_val == 2)
  {
    Serial.println("PIR is Off");
  }else if (NextPIR_val != 2 && NextPIR_val != 1)
  {
    Serial.println("PIR is Not configurated yet");
  }

  //LED status read from EEPROM
  NextLED_val = EEPROM.read(1);
  NextLedIntensity_val = EEPROM.read(4);  
  if (NextLED_val == 1 || NextLedIntensity_val > 3)
  {
    Serial.println("LED is ON");
  }else if (NextLED_val == 2 || NextLedIntensity_val == 3)
  {
    Serial.println("LED is Off");
  }else if (NextLED_val != 2 && NextLED_val != 1)
  {
    Serial.println("LED is Not configurated yet");
  }

  //REED status read from EEPROM
  NextREED_val = EEPROM.read(2);  
  if (NextREED_val == 1)
  {
    Serial.println("REED is ON");
  }else if (NextREED_val == 2)
  {
    Serial.println("REED is Off");
  }else if (NextREED_val != 2 && NextREED_val != 1)
  {
    Serial.println("REED is Not configurated yet");
  }
  
  //BUZZER status read from EEPROM
  NextBUZZER_val = EEPROM.read(3);  
  if (NextBUZZER_val == 1)
  {
    Serial.println("BUZZER is ON");
  }else if (NextBUZZER_val == 2)
  {
    Serial.println("BUZZER is Off");
  }else if (NextBUZZER_val != 2 && NextBUZZER_val != 1)
  {
    Serial.println("BUZZER is Not configurated yet");
  }
  //Led Intensity read from EEPROM
  
  if (NextLedIntensity_val == 3)
  {
    Serial.println("Led Intensity is 0%");
  }else if (NextLedIntensity_val == 4)
  {
    Serial.println("Led Intensity is 25%");
  }else if (NextLedIntensity_val == 5)
  {
    Serial.println("Led Intensity is 50%");
  }else if (NextLedIntensity_val == 6)
  {
    Serial.println("Led Intensity is 75%");
  }else if (NextLedIntensity_val == 7)
  {
    Serial.println("Led Intensity is 100%");
  }else if(NextLedIntensity_val < 3 || NextLedIntensity_val > 7)
  {
    Serial.println("Led Intensity is Not configurated yet");
  }

  //Nextion Display Backlight Intensity
  NextDispBlk = EEPROM.read(5); 
  if(NextDispBlk == 25)
  {
    Serial.println("Nextion display Backlight level is 25%.");
  }else if(NextDispBlk == 50)
  {
    Serial.println("Nextion display Backlight level is 50%.");
  }else if(NextDispBlk == 75)
  {
    Serial.println("Nextion display Backlight level is 75%.");
  }if(NextDispBlk == 100)
  {
    Serial.println("Nextion display Backlight level is 100%.");
  }
  NextNumCrds = EEPROM.read(6);
  if(NextNumCrds == 0)
  {
    Serial.println("No cards in EEPROM.");
  }else if(NextNumCrds == 1)
  {
    Serial.print("EEPROM Contains Cards.");
  }
  
  
}
void NextionReadArduinoEEPROM2()
{
  //PIR data is EEPROM address 0
  //LED data is EEPROM address 1
  //REED data is EEPROM address 2
  //BUZZER data is EEPROM address 3
  //Nextion Display Backlight is EEPROM address 5
  //
  //
  //
  byte NextPIR_val, NextLED_val, NextREED_val, NextBUZZER_val, NextDispBlk_val = 0;

  //PIR status read from EEPROM
  NextPIR_val = EEPROM.read(0);  
  if (NextPIR_val == 1)
  {
    Serial3.print("page1.pirstate.val=");  
    Serial3.print(NextPIR_val);
    Serial3.print("\xFF\xFF\xFF");
  }else if (NextPIR_val == 2)
  {
    Serial3.print("page1.pirstate.val=");  
    Serial3.print(NextPIR_val);
    Serial3.print("\xFF\xFF\xFF");
  }

  //LED status read from EEPROM
  NextLED_val = EEPROM.read(1);  
  if (NextLED_val == 1)
  {
    Serial3.print("page1.ledstate.val=");  
    Serial3.print(NextLED_val);
    Serial3.print("\xFF\xFF\xFF");
  }else if (NextLED_val == 2)
  {
    Serial3.print("page1.ledstate.val=");  
    Serial3.print(NextLED_val);
    Serial3.print("\xFF\xFF\xFF");
  }

  //REED status read from EEPROM
  NextREED_val = EEPROM.read(2);  
  if (NextREED_val == 1)
  {
    Serial3.print("page1.reedstate.val=");  
    Serial3.print(NextREED_val);
    Serial3.print("\xFF\xFF\xFF");
  }else if (NextREED_val == 2)
  {
    Serial3.print("page1.reedstate.val=");  
    Serial3.print(NextREED_val);
    Serial3.print("\xFF\xFF\xFF");
  }
  
  //BUZZER status read from EEPROM
  NextBUZZER_val = EEPROM.read(3);  
  if (NextBUZZER_val == 1)
  {
    Serial3.print("page1.buzzerstate.val=");  
    Serial3.print(NextBUZZER_val);
    Serial3.print("\xFF\xFF\xFF");
  }else if (NextBUZZER_val == 2)
  {
    Serial3.print("page1.buzzerstate.val=");  
    Serial3.print(NextBUZZER_val);
    Serial3.print("\xFF\xFF\xFF");
  }
  //Nextion Display Backlight Intensity
  NextDispBlk_val = EEPROM.read(5);


}
void writeTo(int chAddress, unsigned int ceAddress, byte wData)
{
  EEPROM_BLINK_COLOR_CHANGE = 1;
  Wire.beginTransmission(chAddress);
  Wire.write((int) (ceAddress >> 8));
  Wire.write((int) (ceAddress & 0xFF));
  Wire.write(wData);
  Wire.endTransmission();
  delay(10);
}
byte readFrom(int chAddress, unsigned long ceAddress)
{
  EEPROM_BLINK_COLOR_CHANGE = 2;
  Wire.beginTransmission(chAddress);
  Wire.write((int) (ceAddress >> 8));
  Wire.write((int) (ceAddress & 0xFF));
  Wire.endTransmission();
  Wire.requestFrom(chAddress, 1);

  byte rData = 0;
  if(Wire.available())
  {
    rData = Wire.read();
  }
  return rData;
}
void EEPROMReadOutAll()
{
  eeprom_All_Card_Data_Bytes_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont);

  for (unsigned long i = 0; i < eeprom_All_Card_Data_Bytes_Cont; i++)
  //for (unsigned long i = 0; i < 1200; i++)
  {
    wdt_reset();
    //writeTo(chipAddress, i, 0);
    EEPROM_Current_TIME = millis();
    Serial.print(i); Serial.print(". address data is: "); Serial.println(readFrom(chipAddress, i), HEX);
    EEPROM_RGB_BLINK();
  }
  eeprom_All_Card_Data_Bytes_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  Serial.print("Last Stored UID byte address in EEPROM at INIT: "); Serial.println(eeprom_All_Card_Data_Bytes_Cont);
  Serial.print("Last Stored UID byte address in Arduino EEPROM at INIT: "); Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
  unsigned long INIT_EEPROM_NUM_UID = readFrom(chipAddress, eeprom_Card_Num);
  Serial.print("Number of Cards in EEPROM at INIT: "); Serial.println(INIT_EEPROM_NUM_UID);
  Serial.print("Number of Cards in Arduino EEPROM at INIT: "); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
  //EEPROM_RGB_FINISH();
}
void EEPROMDeleteAll()
{
  for (unsigned long i = 0; i < EEPROM_Size-127900; i++)
  {
    EEPROM_Current_TIME = millis();
    writeTo(chipAddress, i, 0xFF);
    EEPROM_RGB_BLINK();
    //Serial.print(i); Serial.print(". address data is: "); Serial.println(readFrom(chipAddress, i), DEC);
    //EEPROM_RGB_BLINK();
  }
  //EEPROM_RGB_FINISH();
}
void EEPROM_RGB(int redlight_value, int greenlight_value, int bluelight_value)
{
  analogWrite(REDLIGHT_PIN, redlight_value);
  analogWrite(GRRENLIGHT_PIN, greenlight_value);
  analogWrite(BLUELIGHT_PIN, bluelight_value);
}
void EEPROM_RGB_BLINK()
{
  EEPROM_RGB(0, 255, 0);
  WS2812BYellow();
  //EEPROM_Write_To
  if(EEPROM_BLINK_COLOR_CHANGE == 1)
  { 
      if((EEPROM_BLINK_BOOL) && (EEPROM_Current_TIME - EEPROM_Passed_TIME >= EEPROM_TIME_DIF))
      {
        EEPROM_BLINK_BOOL = false;
        //Yellow
        EEPROM_RGB(0, 255, 0);
        EEPROM_Passed_TIME = EEPROM_Current_TIME;
      }else if((!EEPROM_BLINK_BOOL) && (EEPROM_Current_TIME - EEPROM_Passed_TIME >= EEPROM_TIME_DIF))
      {
        EEPROM_BLINK_BOOL = true;
        EEPROM_RGB(0, 0, 0);
        EEPROM_Passed_TIME = EEPROM_Current_TIME;
      }
  //EEPROM_READ_From
  }else if(EEPROM_BLINK_COLOR_CHANGE == 2)
  {
    if((EEPROM_BLINK_BOOL) && (EEPROM_Current_TIME - EEPROM_Passed_TIME >= EEPROM_TIME_DIF))
    {
      EEPROM_BLINK_BOOL = false;
      //Blue
      EEPROM_RGB(0, 0, 255);
      EEPROM_Passed_TIME = EEPROM_Current_TIME;
    }else if((!EEPROM_BLINK_BOOL) && (EEPROM_Current_TIME - EEPROM_Passed_TIME >= EEPROM_TIME_DIF))
    {
      EEPROM_BLINK_BOOL = true;
      EEPROM_RGB(0, 0, 0);
      EEPROM_Passed_TIME = EEPROM_Current_TIME;
    }
  }
}
void EEPROM_RGB_FINISH()
{
  Serial.println("itt");
  //FastLED.clear();
  EEPROM_RGB(0, 255, 0);
  SuccessProgramingBuzzer();
  EEPROM_RGB_BLINK_BOOL = false;
  FastLED.clear();
  sinelon();
}
void EEPROM_TIME_OUT_F()
{
  EEPROM_TIME_OUT = millis();
  if (EEPROM_TIME_OUT >= EEPROM_TIME_WAIT + EEPROM_TIME_INTERVAL)
  {
    EEPROM_TIME_WAIT = EEPROM_TIME_OUT;
    eeprom_All_Card_Data_Bytes_Cont = 0;
    writeTo(chipAddress, eeprom_All_Card_Data_Bytes, eeprom_All_Card_Data_Bytes_Cont);
    Serial.println("Released EEPROM Card Address Byte");
  }
}
void SuccessProgramingBuzzer()
{
  for(int i = 0; i < 4; i++)
    {
        tone(Buzzer, 2000);        
        delay(70);
        noTone(Buzzer);
        delay(70);
    }
}
void ACCOffline()
{
  //EEPROMReadOutAll();
  unsigned long NumofCardsEEprom = readFrom(chipAddress, eeprom_Card_Num);
  Serial.print("Stored Cards are: "); Serial.println(NumofCardsEEprom);
  Serial.print("Stored Cards in Arduino EEPROM are: "); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
  unsigned long NumofLastCardAdd = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  Serial.print("Last Stored UID byte address in EEPROM: "); Serial.println(NumofCardsEEprom);
  Serial.print("Last Stored UID byte address in Arduino EEPROM: "); Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
  uint8_t ActualByteFromEEprom = 0;
  //Serial.println(NumofCardsEEprom);
  //Serial.print("Last card eeprom address I: "); Serial.println(NumofLastCardAdd);
  //Serial.print("Actual Card Data is: ");
  unsigned long Address = 0;

  //Serial.println();
  //Serial.println(sizeof(NumofLastCardAdd));
  for (unsigned long i = 0; i < NumofLastCardAdd; i++)
  {
    //Serial.println(i);
    ActualByteFromEEprom = readFrom(chipAddress, Address);
    //Serial.print(i);
    //Serial.print("0 Actual eeprom value is: ");
    //Serial.println(ActualByteFromEEprom, HEX);

    if(uidDataPackage[2] == ActualByteFromEEprom)
    {
      Address++;
      ActualByteFromEEprom = readFrom(chipAddress, Address);
      //Serial.print(i);
      //Serial.print("1 Actual eeprom value is: ");
      //Serial.println(ActualByteFromEEprom, HEX);
      if(uidDataPackage[3] == ActualByteFromEEprom)
      {
        Address++;
        ActualByteFromEEprom = readFrom(chipAddress, Address);
        //Serial.print(i);
        //Serial.print("2 Actual eeprom value is: ");
        //Serial.println(ActualByteFromEEprom, HEX);
        if(uidDataPackage[4] == ActualByteFromEEprom)
        {
          Address++;
          ActualByteFromEEprom = readFrom(chipAddress, Address);
          //Serial.print(i);
          //Serial.print("3 Actual eeprom value is: ");
          //Serial.println(ActualByteFromEEprom, HEX);
          if(uidDataPackage[5] == ActualByteFromEEprom)
          {
            //mehet a nyitás
            //Serial.print(i);
            //Serial.print(" 4 Actual eeprom value is: ");
            //Serial.println(ActualByteFromEEprom, HEX);
            //It means authorized person
            uidDataPackage[6] = 18;
            LedBuzzerAccessGranted();
            for(unsigned int i = 0; i < sizeof(uidDataPackage); i++)
            {
              Serial.print(uidDataPackage[i], HEX);
            }
            Serial.println();
            Serial.println("Access Granted, door has just opened!");
            Serial.println("Waiting for an ISO14443A Card ...");
            return;
          }Address++;
        }Address++;
      }Address++;
    }Address++;
  }
  for (int i = 0; i < 3; i++)
  {
    LedBuzzerAccessDenied();
  }
  //It's for unauthorized person
  uidDataPackage[6] = 19;
  Serial.println("Access Denied!");
  Serial.println("Waiting for an ISO14443A Card ...");
  
  return;
}
void EEPROM_Vansiher()
{
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
  Serial.print("Number of stored cards in EEPROM: "); Serial.println(eeprom_Card_Num_Cont);
  Serial.print("Number of stored cards in Arduino EEPROM: "); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
  eeprom_All_Card_Data_Bytes_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  Serial.print("Number of stored cards byte address: "); Serial.println(eeprom_All_Card_Data_Bytes_Cont);
  Serial.print("Number of stored cards byte address in Arduino EEPROM: "); Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
  unsigned long i = 0;
  while ( i < eeprom_All_Card_Data_Bytes_Cont)
  {
    writeTo(chipAddress, i, 0);
    Serial.print(i); Serial.print(". "); Serial.println(readFrom(chipAddress, i));
    i++;
  }
  EEPROM.put(8, 0);
  writeTo(chipAddress, eeprom_Card_Num, 0);
  
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
  Serial.print("Number of cards are after EEPROM delete:"); Serial.println(eeprom_Card_Num_Cont);
  Serial.print("Number of cards are after Arduino EEPROM delete:"); Serial.println(EEPROM.get(8, Ardueeprom_card_num_cont));
  EEPROM.put(7, 0);
  writeTo(chipAddress, eeprom_All_Card_Data_Bytes, 0);
  
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_All_Card_Data_Bytes);
  Serial.print("Number of stored cards byte address after EEPROM delete:"); Serial.println(eeprom_Card_Num_Cont);
  Serial.print("Number of stored cards byte address after Arduino EEPROM delete:"); Serial.println(EEPROM.get(7, Ardueeprom_all_Card_data_bytes_cont));
  //Set Number of Card to zero in Arduion EEPROM
  Serial.print("No Cards in ARDU EEPROM after delete");
  EEPROM.write(6, 0);
  SendEEPROM_conf_to_Nextion();

}
void InsideEEPROM_Conf_for_Nextion()
{
  //PIR data is EEPROM address 0
  //LED data is EEPROM address 1
  //REED data is EEPROM address 2
  //BUZZER data is EEPROM address 3
  //Led intensity is EEPROM address 4
    //0% Led intensity is 3
    //25% Led intensity is 4
    //50% Led intensity is 5
    //75% Led intensity is 6
    //100% Led intensity is 7

  //1 - ON, 2 - Off, 0 - Not configurated/Empty

  //Nextion Display Backlight is EEPROM address 5
    //25% of Backlight is 25
    //50% of Backlight is 50
    //75% of Backlight is 75
    //100% of Backlight is 100
  //Address of cards status in EEPROM is address 6


  //Default Settings
  EEPROM.write(0, 2);
  EEPROM.write(1, 1);
  EEPROM.write(2, 1);
  EEPROM.write(3, 1);
  EEPROM.write(4, 7);
  EEPROM.write(5, 100);
  EEPROM.write(6, 0);


}
void SendEEPROM_conf_to_Nextion()
{
  byte led_conf_val, pir_conf_val, reed_conf_val, buzzer_conf_val, led_conf_intesity_val, next_conf_dispblk_val, num_of_cards_eeprom_val = 0;
  led_conf_val = EEPROM.read(1);
  pir_conf_val = EEPROM.read(0);
  reed_conf_val = EEPROM.read(2);
  buzzer_conf_val = EEPROM.read(3);
  led_conf_intesity_val = EEPROM.read(4);
  next_conf_dispblk_val = EEPROM.read(5);
  num_of_cards_eeprom_val = EEPROM.read(6);
  eeprom_Card_Num_Cont = readFrom(chipAddress, eeprom_Card_Num);
  unsigned int Ardu_EEPROM_Unsigned_Int;
  EEPROM.get(8, Ardu_EEPROM_Unsigned_Int);
  
  
  //Send Setup EEPROM data to Nextion
  //delay(1000);

  //Send LED status
  Serial3.print("page1.ledstate.val=");  
  Serial3.print(led_conf_val);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.println(EEPROM.read(1));
  Serial.print("Led status from EEPROM: "); Serial.println(led_conf_val);

  //Send PIR status
  Serial3.print("page1.pirstate.val=");
  Serial3.print(pir_conf_val);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.println(EEPROM.read(0));
  Serial.print("Pir status from EEPROM: "); Serial.println(pir_conf_val);

  //Send REED status
  Serial3.print("page1.reedstate.val=");
  Serial3.print(reed_conf_val);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.println(EEPROM.read(2));
  Serial.print("Reed status from EEPROM: "); Serial.println(reed_conf_val);

  //Send BUZZER status
  Serial3.print("page1.buzzerstate.val=");
  Serial3.print(buzzer_conf_val);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.println(EEPROM.read(3));
  Serial.print("Buzzer status from EEPROM: "); Serial.println(buzzer_conf_val);

  //Send Led_intensity status
  Serial3.print("page3.ledintensity.val=");
  Serial3.print(led_conf_intesity_val);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.println(EEPROM.read(4));
  Serial.print("Led intensity status from EEPROM: "); Serial.println(led_conf_intesity_val);
  //Send Nextion Display Backlight intensity status to Nextion
  Serial3.print("page5.DBright.val=");
  Serial3.print(next_conf_dispblk_val);
  Serial3.print("\xFF\xFF\xFF");
  Serial.print("Display Backlight intensity status from EEPROM is send to nextion: "); Serial.println(next_conf_dispblk_val);
  
  //Send Number of Cards to Nextion
  //Using bit shifting because of unsigned long int variable
  unsigned char eeprom_Card_Num_Cont_byteArray[4];
  // convert from an unsigned long int to a 4-byte array
  /*eeprom_Card_Num_Cont_byteArray[0] = (int)((eeprom_Card_Num_Cont >> 24) & 0xFF) ;
  eeprom_Card_Num_Cont_byteArray[1] = (int)((eeprom_Card_Num_Cont >> 16) & 0xFF) ;
  eeprom_Card_Num_Cont_byteArray[2] = (int)((eeprom_Card_Num_Cont >> 8) & 0XFF);
  eeprom_Card_Num_Cont_byteArray[3] = (int)((eeprom_Card_Num_Cont & 0XFF));*/
  int ConvEpprom_Card_Num = (int)eeprom_Card_Num_Cont;
  Serial3.print("page4.n7.val=");
  Serial3.print(ConvEpprom_Card_Num);
  //Serial3.print(eeprom_Card_Num_Cont_byteArray[3]);
  Serial3.print("\xFF\xFF\xFF");
  //Serial.print("Number of cards sent to Nextion: "); Serial.println(eeprom_Card_Num_Cont_byteArray[3]);
  Serial.print("Number of cards sent to Nextion: "); Serial.println(ConvEpprom_Card_Num);
  Serial.print("Number of cards in Arduino EEPROM is: "); Serial.println(EEPROM.get(8, Ardu_EEPROM_Unsigned_Int));
}
void RTCAdjustNextion()
{
  TimeToNextion();
  //Year
  Serial3.print("page7.n6.val=");
  Serial3.print(DS3231Time[0]+2000);
  Serial3.print("\xFF\xFF\xFF");
  //Month
  Serial3.print("page7.n7.val=");
  Serial3.print(DS3231Time[1]);
  Serial3.print("\xFF\xFF\xFF");
  //Day
  Serial3.print("page7.n8.val=");
  Serial3.print(DS3231Time[2]);
  Serial3.print("\xFF\xFF\xFF");
  //Hour
  Serial3.print("page7.n9.val=");
  Serial3.print(DS3231Time[4]);
  Serial3.print("\xFF\xFF\xFF");
  //Minute
  Serial3.print("page7.n10.val=");
  Serial3.print(DS3231Time[5]);
  Serial3.print("\xFF\xFF\xFF");
  //Second
  Serial3.print("page7.n11.val=");
  Serial3.print(DS3231Time[6]);
  Serial3.print("\xFF\xFF\xFF");

  //rtc.adjust("2015 11 06 23 50 20");
  
}