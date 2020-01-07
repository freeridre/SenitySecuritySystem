#include <Arduino.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "Nextion.h"
#include "SparkFun_Qwiic_Rfid.h"
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <FastLED.h>
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
uint8_t hue = 0;
void sinelon();
uint8_t gHue = 50;
SoftwareSerial FingerPrintSerial(12, 13); // RX, TX
Adafruit_Fingerprint FingerPrint = Adafruit_Fingerprint(&FingerPrintSerial);
SoftwareSerial HC05JDY30(10, 11); // RX, TX

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
//BUZZER INIT
int On = 255, Off = 0, Standby = 50;
int BuzzerOn = 0, BuzzerOff = 255;
int BuzzerFrequency1 = 2000; int BuzzerFrequency2 = 2100; int BuzzerFrequency3 = 2200; int BuzzerFrequency4 = 2300; int BuzzerFrequency5 = 2400;
int Buzzer = 31;
int RedPin = 2;
int GreenPin = 4;
int BluePin = 3;
int MagneticLock = 5;
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
//LoRa Seetings
#define LORA_RESET 111
//How much time after Motion detect.
unsigned int PowerOnTime = 100;
//MagnetReed
const int Reed = 9;
int ReedState;
unsigned long SecondTime;
//How much time after opened door
unsigned long WaitingOpenedDoor = 150;
//Enable opened door detection 0 - no, 1 - BuzzerWithLeds, 2 - noBuzzer
byte DoorOpenedEnable = 1;
unsigned long FirstTime;
unsigned long DoorTimeOut;
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
void LoRaReset ()
{
  Serial1.write(LORA_RESET);
  Serial.println("LoRa Has Just Reseted. Cause: Arduino Gate Has Reseted.");
  Serial1.flush();
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
  }    
  delay(50);  // This delay sets speed of the fade. I usually do from 5-75 but you can always go higher.




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
  return 1;
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
    FastLED.clear(leds);
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
    if (DoorOpenedEnable == 1)
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
  if (DoorOpenedEnable == 1)
  {  
    
    ReedState = digitalRead(Reed);
    if (ReedState == HIGH)
    {
      if (DoorOpenStatusForWS2812B != 1 )
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
      DoorOpenStatusForWS2812B = 0;
      //sinelon();
      //analogWrite(Buzzer, BuzzerOff);

      //noTone(Buzzer);

      /*Serial.print(DoorTimeOut);
      Serial.println(" Locked");*/
      return ReturnDoorTimeOut = 0;
    }
  }else if (DoorOpenedEnable == 0)
  {

  }else if (DoorOpenedEnable == 2)
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
}
void loop()
{
  noTone(Buzzer);
  //FastLED.delay(1000/FRAMES_PER_SECOND);
  //EVERY_N_MILLISECONDS(100);
  //**Serial.println(CountPir);
  currentMillis = millis();
  MOTIONPIR(PIRStateControl);
  ReturnMOTIONPIR = MOTIONPIR(PIRStateControl);
  //Serial.print("ReturnMOTIONPIR: "); Serial.println(ReturnMOTIONPIR);
  int ReturnReturnDoorTimeOut = DoorOpenedTimeOut(ReturnDoorTimeOut);
  if(ReturnReturnDoorTimeOut == 0)
  {
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
  }
  if(cardreading())
    {
      senddatatolora();
    }
  RecieveDataFromGateLoRa();
  ButtonPushedOutSide();
  HC05JDY30FUNCTION();
  DoorOpenedTimeOut(ReturnDoorTimeOut);
  FastLED.show(leds);
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
    // configure board to read RFID tags
    // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
    // 'uid' will be populated with the UID, and uidLength will indicate
    // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    nfc.SAMConfig();
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);//, timeout = 50);
    // Display some basic information about the card
    if (uid[1] > 0)
      {
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        Serial.println("");
        while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength))
        {
          
          WS2812BBlue();
          FastLED.show();
        }
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
    Serial1.write(uid, uidLength);
    Serial.println("UID HAS SENT TO LORA!");
    for (unsigned int i; i < sizeof(uid); i++)
    {
      uid[i] = 0;
    }
    uidLength = 0;
    for (unsigned int i; i < sizeof(uid); i++)
    {
      int UIDDelete = uid[i];
      Serial.print(UIDDelete, HEX);
    }
    Serial.print("\n");
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
        Serial.println("Access Granted, door has just opened by Button!");
        Serial.println("Waiting for an ISO14443A Card ...");
        ButtonValToDoorControl = 0;
    }
  }
}
