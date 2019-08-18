#include <Arduino.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "Nextion.h"
#include "SparkFun_Qwiic_Rfid.h"
#include <SoftwareSerial.h>
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
int On = 255, Off = 0;
int BuzzerOn = 0, BuzzerOff = 255;
int Buzzer = 6;
int RedPin = 2;
int GreenPin = 3;
int BluePin = 4;
int MagneticLock = 5;
int Button = 7;
int ButtonVal;
int ButtonValToDoorControl;
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
unsigned long currentMillis = millis();
unsigned long time_half_second = 5000;
boolean cardreading ();
void NFCINITIALIZE ();
void senddatatolora();
void ButtonPushedOutSide();
void HC05JDY30FUNCTION();
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
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(MagneticLock, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  analogWrite(Buzzer, BuzzerOn);
  pinMode(Button, INPUT);
  delay(100);
  if (currentMillis < time_half_second)
  {
    analogWrite(Buzzer, BuzzerOff);
    setColor(Off, Off, On);
    }
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Hello!");
  Wire.begin();
  HC05JDY30.begin(9600);
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
      Serial.println("Waiting for an ISO14443A Card ...");
    }
    if (ControlData == 11)
    {
      for (int i = 0; i < 3; i++)
      {
        LedBuzzerAccessDenied();
      }
      Serial.println("Access Denied!");
      Serial.println("Waiting for an ISO14443A Card ...");
    }
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
void loop() {
  //setColor(Off, Off, On);
  if(cardreading ())
  {
    Serial.println("Itt vagyok");
    delay(1110);
    senddatatolora();
  }
  RecieveDataFromGateLoRa();
  ButtonPushedOutSide();
  HC05JDY30FUNCTION();
  //setColor(Off, Off, Off);
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
boolean ret;

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
    for (unsigned int u; u < TagLength; u++)
    {
      IntegerDataStringArray[u] = 0;
    }
    TagLength = 0;
    tag = "";
    ret = true;
    return ret;
  }
  // configure board to read RFID tags
  nfc.SAMConfig();
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);//, timeout = 50);
  // Display some basic information about the card
  if (uid[1] > 0)
    {
      Serial.println("Found an ISO14443A card");
      Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      Serial.print("  UID Value: ");
      nfc.PrintHex(uid, uidLength);
      Serial.println("");
      ret = true;
      return ret;
    }
    ret = false;
    return ret;
  }
  void senddatatolora()
  {
    Serial1.write(uid, uidLength);
    Serial.println("UID HAS SENT TO LORA!");
    for (int i; i < 6; i++)
    {
      uid[i] = 0;
    }
    uidLength = 0;
    for (int i; i < 6; i++)
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
