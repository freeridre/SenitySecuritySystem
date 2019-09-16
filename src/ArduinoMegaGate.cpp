#include <Arduino.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "Nextion.h"
#include "SparkFun_Qwiic_Rfid.h"
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
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
int Buzzer = 6;
int RedPin = 2;
int GreenPin = 3;
int BluePin = 4;
int MagneticLock = 5;
int Button = 7;
int ButtonVal;
int ButtonValToDoorControl;
int PIR = 8;
int PIRState = 0;
unsigned int PIRCount = 0;
unsigned int PIRStat2 = 0;
unsigned int PIRStateControl, ReturnMOTIONPIR;
unsigned int WaitingPIR, WaitingPIR2, WaitingCardReading, WaitingCardReading2, osszeg = 0, CountDownReader = 3;
unsigned int PowerOnTime = 50;
//MagnetReed
const int Reed = 9;
int ReedState;
unsigned long SecondTime;
unsigned long WaitingOpenedDoor = 150;
unsigned long FirstTime;
unsigned long DoorTimeOut;
unsigned long ReturnDoorTimeOut;
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
    Serial.println("No Motion!");
    PIRStateControl = 0;
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
    analogWrite(Buzzer, BuzzerOff);
    setColor(Off, Off, Off);
    //Serial.println("AJJASJF");
    DoorOpenState = false;
  }
  else if ((!DoorOpenState) && (currentMillis - previousMillis) >= OpenedDoorOffTime)
  {
    //ledState = HIGH;  // turn it on
    previousMillis = currentMillis;   // Remember the time
    //digitalWrite(ledPin, ledState);	  // Update the actual LED
    analogWrite(Buzzer, BuzzerOn);
    setColor(On, On, Off);
    //Serial.println("125235163262");
    DoorOpenState = true;
  }
  /*setColor(On, On, Off);
  analogWrite(Buzzer, BuzzerOn);
  delay(250);
  setColor(Off, Off, Off);
  analogWrite(Buzzer, BuzzerOff);
  delay(250);*/
}
void LedBuzzerAccessGranted()
{
  setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  analogWrite(Buzzer, BuzzerOn);
  delay(150);
  setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  analogWrite(Buzzer, BuzzerOff);
  delay(150);
  setColor(Off, On, Off);
  analogWrite(MagneticLock, On);
  analogWrite(Buzzer, BuzzerOn);
  delay(1000);
  setColor(Off, Off, Off);
  analogWrite(MagneticLock, Off);
  analogWrite(Buzzer, BuzzerOff);
  delay(150);
  //Blue
  setColor(Off, Off, On);
}
void LedBuzzerAccessDenied()
{
setColor(On, Off, Off);
  analogWrite(Buzzer, BuzzerOn);
  delay(150);
  setColor(Off, Off, Off);
  analogWrite(Buzzer, BuzzerOff);
  delay(150);
  //Blue
  setColor(Off, Off, On);
}
void setup()
{
  FirstTime = millis();
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(MagneticLock, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  analogWrite(Buzzer, BuzzerOn);
  pinMode(Button, INPUT);
  pinMode(PIR, INPUT);
  pinMode(Reed, INPUT_PULLUP);
  delay(100);
  if (currentMillis < time_half_second)
  {
    analogWrite(Buzzer, BuzzerOff);
    setColor(Off, Off, Standby);
    }
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Hello!");
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
  ReedState = digitalRead(Reed);
  if (ReedState == HIGH)
  {
    ReedState = digitalRead(Reed);
    DoorTimeOut++;
    /*Serial.print(DoorTimeOut);
    Serial.println(" Opened");*/
    if (DoorTimeOut >= WaitingOpenedDoor)
    {
      ReedState = digitalRead(Reed);
      DoorTimeOut--;
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
    analogWrite(Buzzer, BuzzerOff);
    /*Serial.print(DoorTimeOut);
    Serial.println(" Locked");*/
    return ReturnDoorTimeOut = 0;
  }
}
void loop()
{
  currentMillis = millis();
  MOTIONPIR(PIRStateControl);
  ReturnMOTIONPIR = MOTIONPIR(PIRStateControl);
  Serial.print("ReturnMOTIONPIR: "); Serial.println(ReturnMOTIONPIR);
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
      setColor(Off, Off, On);
      PIRCount++;
      //Serial.print("PIRCount: "); Serial.println(PIRCount);
      if (PIRCount >= PowerOnTime)
      {
        PIRState = digitalRead(PIR);
        PIRCount--;
        setColor(Off, Off, Standby);
        //Serial.print("PIRCount: "); Serial.println(PIRCount);
        PIRStat2 = 0;
        PIRCount = 0;
      }
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
  //Serial.println("Waiting for a Card...");
}
  boolean cardreading ()
{
  //Serial.println("ITT2");
  boolean ret = false;
  /*MOTIONPIR(PIRStateControl);
  ReturnMOTIONPIR = MOTIONPIR(PIRStateControl);
  if (ReturnMOTIONPIR == 1)
  {*/
      tag = myRfid.getTag();
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
      }
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
        {}
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
