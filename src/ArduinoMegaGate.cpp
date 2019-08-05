#include <Arduino.h>
//ARDUINO SIDE kapu
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "Nextion.h"

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
void loop() {
  //setColor(Off, Off, On);
  if(cardreading ())
  {
    senddatatolora();
  }
  RecieveDataFromGateLoRa();
  ButtonPushedOutSide();
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
  Serial.println("Waiting for an ISO14443A Card ...");
}
  boolean cardreading ()
  {
  // configure board to read RFID tags
  nfc.SAMConfig();
  boolean ret;
  
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
    for (int i; i < 6; i++)
    {
      int UIDDelete = uid[i];
      Serial.print(UIDDelete, HEX);
    }
    Serial.print("\n");
    delay(1110);
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
