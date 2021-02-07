//ARDUINO INSIDE
//COMS
//Arduino Outside COM7
//Arduino Inside COM5
//LORA Outside COM
//LORA Inside COM21
//Include libraries

//////////////////////
//NTP SETTINGS
//#include <Ethernet.h>
//#include <EthernetUdp.h>
#include <Time.h>
/////////////////////
#include "arduino.h"
#include <avr/wdt.h>
#include "Wire.h"
#include "SPI.h"
#include "EEPROM.h"
#include "Adafruit_PN532.h"
#include "SparkFun_Qwiic_Rfid.h"
#include <RTClib.h>
RTC_DS3231 rtc;
uint8_t RTCData[7] = {0, 0, 0, 0, 0, 0, 0};
unsigned long NextionCurrentTime;
unsigned long NextionStartTime;
unsigned int NextionTimeInterval = 250;
void TimeNow();
void TIMEAdjust();
void RTC_F();
void SendTimeToNextion();
DateTime now;
//Define SPI Pins
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
//LoRa Reset
#define LORA_RESET 112
//Make NFC object
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
//Card Loading --4096 Bytes--
/*Maga az EEPROM méretét határozzuk itt meg (max 4096)*/ int eeAddresseeprom = 4000;
/*Hány db kártya lett elmentve és annak az értékét melyik címre helyezzük el*/ int kartyadbeeprom = 4002;
int eepromcimeeprom = 4001;
//int kartyadba = 1;
/*Hány db kártya lett elmentve*/ int kartyadb;
int eepromcim = 0;
/*Maga az EEPROM méretét az eeAddress változóba raktuk*/ int eeAddress = EEPROM.read(eeAddresseeprom);
//Init NFC Variables
uint8_t success = 0;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength = 0;
uint8_t Timeout = 0;
boolean checknfc;
//byte value;
int ID;
//RFiD variables
#define RFID_ADDR 0x7D // Default I2C address 
Qwiic_Rfid myRfid(RFID_ADDR);
String tag, DataString; 
int IntegerDataString;
int IntegerDataStringArray[16] = {};
int b = 1;
int c = 0;
int o = 0;
int RFIDID = 14;
unsigned int iRFID = 0;
unsigned int TagLength = 0;

int ten = 10;
int eleven = 11;
char CardDeleteUserInput[50];
uint8_t IntegerDataStringArray1[4] = {};
//LoRa
uint8_t byteFromESP32 [18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const uint8_t RetryCardDataDownloadCallBackPackage[3] = {113, 113, 239};
const uint8_t RetryCardDataDownloadStartedBackPackage[3] = {113, 113, 240};
const uint8_t EmptyDBtoGateController[3] = {113, 113, 241};
uint8_t EndOfCardDataFromDBInt[3] = {63, 63, 64};
uint8_t val = 0;
uint8_t CardNUmToLoRa[5] = {42, 42, 45, val, 42};
byte Status;
//const int eeAddress;
//User Serial3 Input
//if input_a == '5' it automatically enters logging mode
int input_a = '5';
boolean logging = true;
//EEPROM
unsigned int address = 0;
byte value;
byte value2;
byte value3;
byte valuekartyadb;
int carddb = 0;
int RFIDControlByte = 0;
bool Exit = Serial3.available();
int back = 1;
//menu Loop()
int main_menu = 1;
String NTP;
void readfromoutsideLoRa();
void CardLearningV2();
void AccessControl();
void CardDeleting();
void CardLearningV3();
void GetCardDataFromMySQLDB();
void CardLearningMaster();
void CardReadOutMySQL();
void CardReadOutMySQLv2();
void SendActionToMysql();
int StrToHex(char str[]);
void ServerNTP();
void SendNTPServerTimeToNextion();
void TimeToNextion();
int CardUpdateToDB();
uint8_t PiNTP[17] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int iu = 0;
int icount;
String CardDataFromDB = "";
String MySQLCardReadOutIndentifier = "4";
String FirstName;
String LastName;
boolean FirstNameBool = false;
boolean LastNameBool = false;
boolean CardLearningName = false;
boolean CardLearningProcess = false;
String MySQLCardDataToDB;
String MySQLSeparator = ",";
String MySQLCardNumbersReturn;
String CardData = "";
String MySQLCardDataToDBID = "";
String MySQLCardDataUpdate = "";
String MySQLCardDataUpdateID;
String UpLastName, UpFirstName;
unsigned long CurrUpdateCancel = millis();
const long UpdateCancelInterval = 20000;
unsigned long PrevUpdateCancel = 0;
unsigned long CurrentUpdateCancelequ;
int UIDCHRT = 0;
int MySQLCardNumber = 0;
int CardChoice2 = 0;
int CardNumberFromDb = 0;
long int MySQLConnectionCurrTimeOut;
long int MySQLConnectionPrevTimeOut;
long int MySQLConnectionTimeOutInterval = 10000;
const long CardUpdateTimeoutInterval = 3000;
unsigned long CardUpdateTimeCurrent;
unsigned long CardUpdateTimePrev = 0;
int Counter = 0;
unsigned int UInput;
//////////////////////////////////////
void accesscontrolV2();
String MySQLAccessControlIdentifier = "3";
//byte Status;
boolean ReadedCard = false;
void accesscontrolmysql();
//Itt is csereltem
char uidchar[6] = "\0";
String uidString = "";
String uidStringend = "*";
String UidStringToMysql = "";
boolean NMC = true;
String UuserID;
/////////////////////////////////////
void LoRaReset()
{
  Serial1.write(LORA_RESET);
  Serial3.println("LoRa Has Just Reseted. Cause: Arduino Inside Has Reseted.");
}
void CardReadOut()
{
  Serial3.println("EEPROM READ OUT ALL ONLY REGISTERED CARDS!\n");
  int a_valuekartyadb = EEPROM.read(kartyadbeeprom) -1;
  valuekartyadb = a_valuekartyadb*5; 
  address = 0;
  for (int i = 0; i<valuekartyadb; i++)
  {
    value = EEPROM.read(address);
    Serial3.print(address);
    Serial3.print("\t");
    Serial3.print("0x");
    Serial3.print(value, HEX);
    Serial3.println();
    address = address + 1;
    if (address == valuekartyadb)
    {
      address = 0;
    }
  }
  /*else if (digitalRead(13) == HIGH){
    address = 0;
    for (int i = 0; i<4; i++){
    value = EEPROM.read(address);
    Serial3.print(address);
    Serial3.print("\t");
    Serial3.print(value, HEX);
    Serial3.println();
    address = address + 1;
      if (address == EEPROM.length()) {
        address = 0;
      }
    delay(5);
    }
    }*/
    Serial3.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
   while (6 != 7)
    {
      input_a = Serial3.read();
        if (input_a == '6')
        {
          main_menu = 1;
          loop();
          return;
        }
    }
}
void EEPROMDELETE()
{
  address = 0;
  value = 0;
  for (unsigned int i = 0; i < EEPROM.length(); i++)
  {
    
    EEPROM.write(i, value);
    Serial3.print(address, DEC);
    Serial3.print("\t");
    Serial3.println(value);
    address = address + 1;
  }
    Serial3.println("A teljes EEPROM torolve lett!");
    kartyadb = 1;
    EEPROM.write(kartyadbeeprom, kartyadb);
    //int eepromcim = EEPROM.read(eepromcimeeprom);
    main_menu = 1;
    loop();
    return;
}
void EEPROMREADOUTALL()
{
  Serial3.println("EEPROM READ OUT ALL!\n");
  address = 0;
  for (unsigned int i = 0; i<EEPROM.length(); i++)
  {
    value = EEPROM.read(address);
    Serial3.print(address);
    Serial3.print("\t");
    Serial3.print(value, HEX);
    Serial3.println("\n");
    address = address + 1;
    if (address == EEPROM.length())
    {
      address = 0;
    }
  }
  //quit
  Serial3.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
   while (6 != 7)
    {
      input_a = Serial3.read();
        if (input_a == '6')
        {
          main_menu = 1;
          loop();
          return;
        }
    }
}
//Start NFC Init
void NFCINITIALIZE ()
{
 Serial3.println("NFC initialization in progress...");
 //Start NFC Comm.
 nfc.begin();
 //Start SAM
 nfc.SAMConfig();
 //Loop PN Reader
 nfc.setPassiveActivationRetries(0x01);
 //Get NFC module GetFirmwareVersion
 uint32_t versiondata = nfc.getFirmwareVersion();
 if (! versiondata)
 {
   Serial3.print("Didn't find PN53x board");
   //halt
   while (1);
 }
 //Print out chipversion
 Serial3.print("Found chip PN5"); Serial3.println((versiondata>>24) & 0xFF, HEX); 
 Serial3.print("Firmware ver. "); Serial3.print((versiondata>>16) & 0xFF, DEC); 
 Serial3.print('.'); Serial3.println((versiondata>>8) & 0xFF, DEC);
 Serial3.print("\n");
}
//Kartyatanitas EEPROM
/*void CardLearning()
{
 do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    Serial3.println("-------------------------------------------------------------------------------");
    Serial3.print("Kerem a""(z) "); Serial3.print(kartyadb); Serial3.print(". taget/kartyat. \n");
    Serial3.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    Serial3.println("A tovabb lepeshez nyomd meg barmelyik gombot (1,2,3,4,5), a visszalepeshez nyomd meg a 6-os gombot!\n");
    //quit
    while (input_a != 7)
    {
      input_a = Serial3.read();
        if (input_a == 6)
        {
          main_menu == 1;
          Serial3.println("||||||||||||||||||||||");
          Serial3.println("1 - Card Learning \n");
          Serial3.println("2 - Card Listing \n");
          Serial3.println("3 - EEPROM Delete \n");
          Serial3.println("4 - EEPROM Read Out All \n");
          Serial3.println("5 - Access Control");
          Serial3.println("||||||||||||||||||||||\n");
          loop();
          return;
        }
        else if ((input_a == 1) || (input_a == 2) || (input_a == 3) ||(input_a == 4) || (input_a == 5))
        {
        break;
        }
    }
    Serial3.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    //Scan RFID UID
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, Timeout);
    
    Serial3.println("A taget/kartyat beolvastuk! \n");
    //delay(1500);
    //kartyadb = EEPROM.read(kartyadbeprom);
    //Leellenorizzuk, hogy mar fel van-e veve ez a UID
    byte value;
    int ID = 0;
    carddb = kartyadb * uidLength;
    address = 0;
    for (int i = 0; i < carddb; i++)
    {
      
      value = EEPROM.read(address);
      if(value == uid[ID])
      {
        Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
        Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
        ID++, address++;
        //Serial3.println("Find the"); Serial3.print(ID); Serial3.print(" byte.");
        value = EEPROM.read(address);
        if(value == uid[ID])
        {
          Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
          Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
          ID++, address++;
          value = EEPROM.read(address);
          if(value == uid[ID])
          {
            Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
            Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
            ID++, address++;
            value = EEPROM.read(address);
            if(value == uid[ID])
            {
              Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
              Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
              ID++, address++;
              value = EEPROM.read(address);
              Serial3.println("Ez a kartya/tag mar fel lett veve! Kerlek valasz egy masikat!");
              delay(5000);
              CardLearning();
              return;
            }
            address++;
            
          }
          address++;
          
        }
        address++;
        
      }
      address++;
    }




    Serial3.print ("A(z) "); Serial3.print(kartyadb); Serial3.print(". TAG/KARTYA UID:"); nfc.PrintHex(uid, uidLength);
    if (uidLength == 4)
    {
      Serial3.println("Ez egy Mifare Classic kartya 4 byte UID.");
      //EEPROM WRITE SCANNED RFID UID
      eeAddress = EEPROM.read(eeAddresseeprom);
      EEPROM.put(eeAddress, uid);
      eepromcim = EEPROM.read(eepromcimeeprom);
      Serial3.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial3.print(eeAddress); Serial3.print(". bytetol kezdve! \n");
      eeAddress=eeAddress+5;
      EEPROM.put(eeAddresseeprom,eeAddress);
      eepromcim=eepromcim+5;
      EEPROM.write(eepromcimeeprom,eepromcim);
      //delay(2000);
      kartyadb++;
      EEPROM.put(kartyadbeeprom, kartyadb);
      delay(2000);
      Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
      Serial3.println("A kovetkezo TAG/KARTYA felvetelehez kerlek erintsd meg a kijelzőt!");
      Serial3.println("A fomenube valo visszajutashoz kerlek nyomd meg a vissza gombot!");
      Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
      Serial3.println("------------------------------------------------------------------------------- \n");
      //eeAddress = EEPROM.read(eeAddresseeprom);
      //ide kell az erintes figyelese
      while ((digitalRead(13) == LOW) && (digitalRead(8) == LOW));
    }
    else if (uidLength == 7)
    {
      Serial3.println("Ez egy Mifare Ultralight kartya 7 byte UID jelenleg nem támogatott!");
      CardLearning();
      //EEPROM WRITE SCANNED RFID UID
      EEPROM.put(eeAddress, uid);
      eeAddress=eeAddress+8;
      EEPROM.put(eeAddresseeprom,eeAddress);
      Serial3.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial3.print(eepromcim); Serial3.print(". bittol kezdve! ");
      eepromcim = eepromcim+8;
      EEPROM.put(eepromcimeeprom,eepromcim);
      //delay(2000);      
      kartyadb++; 
      EEPROM.put(kartyadbeeprom, kartyadb);
      Serial3.println();
      Serial3.println();
      Serial3.println("A kovetkezo TAG/KARTYA felvetelehez kerlek nyomd meg az 1-es gombot!");
      Serial3.println("A fomenube valo visszajutashoz kerlek nyomd meg a 6-os gombot!");
      while ((digitalRead(13) == LOW) && (digitalRead(8) == LOW)); 
    }
    else if (uidLength > 7)
    {
      Serial3.println("A KARTYA/TAG nem kompatibilis!");
      CardLearning();
    }
  }
  while (digitalRead(13) == HIGH);
}
void RelayBuzzer()
{
analogWrite(4, HIGH);
Serial3.println("||||||||||||||||||||||");
Serial3.println("1 - Card Learning \n");
Serial3.println("2 - Card Listing \n");
Serial3.println("3 - EEPROM Delete \n");
Serial3.println("4 - EEPROM Read Out All \n");
Serial3.println("5 - Access Control");
Serial3.println("||||||||||||||||||||||\n");
loop();
return;
}
*/
void AccessControl()
{
    do
    {
      
      Serial3.println("Attempt to open the door at: ");
      
        //Serial3.print(byteFromESP32[o], DEC);
        //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[12], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
      
      nfc.SAMConfig();
      //kártyadb bekerese EEPROMbol, illetve kartya kerese
      kartyadb = EEPROM.read(kartyadbeeprom);
      Serial3.println();
      //Serial3.println("-------------------------------------------------------------------------------");
      //Serial3.print("Kerem a "); Serial3.print("taget/kartyat. \n");
      //Serial3.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
      //Serial3.println("Waiting for incoming Data");

      //Scan RFID UID
      //success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, Timeout);
      //Serial3.println("A taget/kartyat beolvastuk! \n");
      
      //Leellenorizzuk, hogy mar fel van-e veve ez a UID
      byte value;
      //Az első kettő FF, FF
      int ID = 2;
      carddb = kartyadb * uidLength;
      address = 0;
      for (int i = 0; i < carddb; i++)
      {
        value = EEPROM.read(address);
        if(byteFromESP32[ID] == 8)
        {
          Serial3.println("Passed! \n");
          Serial3.println("It's an android Phone!");
                Status = 12;
                digitalWrite(35, HIGH);
                //delay(2000);
                Serial1.write(Status);
                Serial1.flush();
                Serial3.print("Status "); Serial3.print(Status); Serial3.print(" has written back to LoRa! \n");
                digitalWrite(35, LOW);
                for(unsigned int zi = 0; zi < sizeof(byteFromESP32); zi++)
                {
                  byteFromESP32[zi] = 0;
                }
                //Serial3.println("Waiting for incoming data...");
                Serial3.println("-------------------------------------------------------------------------------");
                Serial3.println("Waiting for incoming data...");
                //readfromoutsideLoRa();
                return;
        }
        else if(value == byteFromESP32[ID])
        {
          /*Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
          Serial3.print("A UID "); Serial3.print(ID-1); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");*/
          ID++, address++;
          //Serial3.println("Find the"); Serial3.print(ID); Serial3.print(" byte.");
          value = EEPROM.read(address);
          if(value == byteFromESP32[ID])
          {
            /*Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
            Serial3.print("A UID "); Serial3.print(ID-1); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");*/
            ID++, address++;
            value = EEPROM.read(address);
            if(value == byteFromESP32[ID])
            {
              /*Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
              Serial3.print("A UID "); Serial3.print(ID-1); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");*/
              ID++, address++;
              value = EEPROM.read(address);
              if(value == byteFromESP32[ID])
              {
                /*Serial3.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
                Serial3.print("A UID "); Serial3.print(ID-1); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");*/
                ID++, address++;
                value = EEPROM.read(address);
                Serial3.println("Passed! \n");
                //Itt van a hiba!!
                //RelayBuzzer();
                //vissza a LoRa-ra
                //itt küldi vissza az engedélyező bitet
                Status = 12;
                digitalWrite(35, HIGH);
                //delay(2000);
                Serial1.write(Status);
                Serial1.flush();
                Serial3.print("Status "); Serial3.print(Status); Serial3.print(" has written back to LoRa! \n");
                digitalWrite(35, LOW);
                for(unsigned int zi = 0; zi < sizeof(byteFromESP32); zi++)
                {
                  byteFromESP32[zi] = 0;
                }
                //Serial3.println("Waiting for incoming data...");
                Serial3.println("-------------------------------------------------------------------------------");
                Serial3.println("Waiting for incoming data...");
                //readfromoutsideLoRa();
                return;
                
                /*digitalWrite(36, HIGH);
                delay(100);
                digitalWrite(36, LOW);
                //delay(2000);
                Serial3.println("Waiting for incoming data...");
                readfromoutsideLoRa();
                return;*/
                
                //Serial3.println("STOP!");
              }
              address++;
            }
            address++;
          }
          address++;
        }
        address++;
      }
      Serial3.println("Nem jogosult! \n");
      //delay(2000);
      Status = 11;
      digitalWrite(35, HIGH);
      Serial1.write(Status);
      Serial1.flush();
      Serial3.print("Status "); Serial3.print(Status); Serial3.print(" has written back to LoRa! \n");
      digitalWrite(35, LOW);
      for(unsigned int zi = 0; zi < sizeof(byteFromESP32); zi++)
                {
                  byteFromESP32[zi] = 0;
                }
      Serial3.println("-------------------------------------------------------------------------------");
      Serial3.println("Waiting for incoming data...");

      //readfromoutsideLoRa();
      return;
    }while (10 != 11);
}
void DataFromNTP()
{
  //Serial3.println("Itt vagyok");
  // Update the time via NTP server as often as the time you set at the top
}
void readfromoutsideLoRa()
{
  logging = false;
  wdt_reset();
  NextionCurrentTime = millis();
  UInput = 0;
  //DataFromNTP();
    //Serial3.println("ahahah");
    input_a = Serial3.read();
    if (input_a == '6')
    {
      logging = false;
      main_menu = 1;
      loop();
      //return;
    }
    unsigned int i = 0;
    SendTimeToNextion();
    //ServerNTP();


    while(Serial1.available() > 0)
    {
      
      NextionCurrentTime = millis();
      //Orange Led shows Serial3 communication progress
      SendTimeToNextion();
      //ServerNTP();

      digitalWrite(35, HIGH);
      Serial3.print("First Data Income: ");
      while(Serial1.available() > 0)
      {
        
        byteFromESP32[i] = Serial1.read();
        
        Serial3.print("0x"); Serial3.print(byteFromESP32[i], HEX); Serial3.print(" ");
        
        i++;
        //i += 1;
      }
      /*if(byteFromESP32[0] == 255 && byteFromESP32[1] == 255 && byteFromESP32[16] == 255 && byteFromESP32[17] == 255)
      {
        Serial3.print("0x"); Serial3.print(byteFromESP32[i], HEX); Serial3.print(" ");
      }*/
      //Opened Door
      if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 255)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.print("The Door is Opened at: ");
        ///////
        //Year
        if (byteFromESP32[3+6] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3+6], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4+6], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5+6], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6+6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7+6], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8+6], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9+6], DEC);Serial3.println();
        //////
        //Send data to db
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");

        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
        }
        Serial3.println("Waiting for incoming Data");
      }
      //Closed Door
      else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 254)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.print("The Door is Closed afrter longterm Open Status at: ");
        ///////
        //Year
        if (byteFromESP32[3+6] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3+6], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4+6], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5+6], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6+6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7+6], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8+6], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9+6] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9+6], DEC);Serial3.println();
        //////
        //Send Data to DB
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");


        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
        }
        Serial3.println("Waiting for incoming Data");
      }
      //Button pressed
      else if (byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 21)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.print("The OutSide Button pressed at: ");
        ///////
        //Year
        if (byteFromESP32[3] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9], DEC);Serial3.println();
        //////
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");
        Serial3.println("Waiting for incoming Data");
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        Serial3.println();
        Serial3.println("Waiting for incoming Data");
      }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 22)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.print("Retry Card Data Download Request at: ");
        //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
        //////
        
        //////
        SendActionToMysql();
        /*Serial3.println("Request and Action Sent to DB.");
        Serial1.write(RetryCardDataDownloadStartedBackPackage, sizeof(RetryCardDataDownloadStartedBackPackage));
        //delay(500);
        NMC = true;
        CardReadOutMySQLv2();*/
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        Serial3.println();
        Serial3.println("Waiting for incoming Data");
      }
      else if (byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 33)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.print("Door opened by smartphone app at: ");
        ///////
        //Year
        if (byteFromESP32[3] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9], DEC);Serial3.println();
        //////
        Serial3.println();
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");
        Serial3.println("Waiting for incoming Data");
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        Serial3.println();
        Serial3.println("Waiting for incoming Data");
      }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 34)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.println("Retry Card Data Download Was Success at: ");
        ///////
        //Year
        if (byteFromESP32[3] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9], DEC);Serial3.println();
        //////
        Serial3.println();
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");
        Serial3.println("Waiting for incoming Data");
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        //Serial3.println(i);

      }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 35)
      {
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.println("Gate Reseted at: ");
        ///////
        //Year
        if (byteFromESP32[3] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9], DEC);Serial3.println();
        //////
        Serial3.println();
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");
        Serial3.println("Waiting for incoming Data");
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        //Serial3.println(i);

      }
      else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 23)
      {
        Serial3.print("Master User Signed in at Gate at: ");
        //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
        //////
        
        //////
        SendActionToMysql();
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
      }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 20)
      {
        //i = 1;
        Serial3.print('\n');
        Serial3.print("Bytes received: ");
        Serial3.print(i, DEC);
        Serial3.print('\n');
        Serial3.println("Opened by security reasons at: ");
        ///////
        //Year
        if (byteFromESP32[3] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[3], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[4] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[4], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[5] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[5], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[7] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[7], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[8] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[8], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[9] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[9], DEC);Serial3.println();
        //////
        Serial3.println();
        SendActionToMysql();
        Serial3.println("Action Sent to DB.");
        Serial3.println("Waiting for incoming Data");
        //Release Data
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        //Serial3.println(i);

      }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 23)
      {
        Serial3.print("Master User Signed in at Gate at: ");
        //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
        //////
        
        //////
        SendActionToMysql();
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
      }
      else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 24)
      {
        Serial3.print("Master User Signed in at Gate at: ");
        //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[6], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
        //////
        
        //////
        SendActionToMysql();
        for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
      }
    }
    digitalWrite(35, LOW);
    //Release Data
    input_a = Serial3.read();
    if (input_a == '6')
    {
      
      main_menu = 1;
      loop();
      return;
    }
    //It's goes to Mysql Data Registration and auth
    if ((byteFromESP32[0] == 255 && byteFromESP32[1] == 255 || byteFromESP32[1] == 155) || (byteFromESP32[0] == 0x6F && byteFromESP32[1] == 0x6F ))
    {
      //Serial3.println("Itt vagyok1");
      if(i != 0)
      {
          //Serial3.println("Itt vagyok2");
          Serial3.print('\n');
          Serial3.print("OutSide Reader Bytes received: ");
          Serial3.print(i, DEC);
          Serial3.print('\n');
          Serial3.print('\n');
        //int x = 2;
        uidLength = 0;
        for (int x = 2; x < 8; x++)
        {
        uint8_t uid_lenght = byteFromESP32[x];
          if (uid_lenght > 0)
          {
            uidLength++;
          }
        }
        Serial3.print("UID Lenght: "); Serial3.println(uidLength);
        //uidLength = 0;
         //Release Data
        /*for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          byteFromESP32[z] = 0;
          Serial3.print(byteFromESP32[z]);
        }*/
          
          
          
        //accesscontrolmysql();
        SendActionToMysql();
        
        //AccessControl();

        //return;
      }
    }
    input_a = Serial3.read();
    if (input_a == '6')
    {
      main_menu = 1;
      loop();
      //return;
    }
  //}while(1 > 0);
  readfromoutsideLoRa();
  //return;
}
void setup() {
  wdt_disable();
  wdt_enable(WDTO_8S);
  // put your setup code here, to run once:
  //Serial30 start
  //Pi UART communication
  Serial.begin(115200);
  //UART monitor
  Serial3.begin(115200);
  /////////////////////////////////////////
    //Welcome
  Serial3.println("Welcome! It's Serial3!");
  //Begin I2C
  //**Wire.begin();
  //Test I2C Communication
  if(myRfid.begin())
    Serial3.println("I2C RFiD Ready!"); 
  else
    Serial3.println("Could not communicate with Qwiic RFID!");

  //Serial1 start
  //Lora communication
  Serial1.begin(115200);
  Serial2.begin(115200);
  //TIMEAdjust();
  NextionStartTime = millis();
  if (! rtc.begin())
  {
    Serial3.println("Couldn't find RTC");
  }else
    Serial3.println("RTC Has just Started!");
  //magnes
  pinMode(36, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(4, OUTPUT);
  //LoRa Reset
  LoRaReset();
  //NFC Module Init
  NFCINITIALIZE ();
  /*
  Serial3.println("||||||||||||||||||||||");
  Serial3.println("1 - Card Learning \n");
  Serial3.println("2 - Card Listing \n");
  Serial3.println("3 - EEPROM Delete \n");
  Serial3.println("4 - EEPROM Read Out All \n");
  Serial3.println("5 - Access Control");
  Serial3.println("||||||||||||||||||||||\n");
  */
 
}
void loop()
{
  wdt_reset();
  CardUpdateTimePrev = millis();
  PrevUpdateCancel = millis();
  while(true)
  {
    wdt_reset();
    SendTimeToNextion();
    NextionCurrentTime = millis();
    if (main_menu == 1)
    {
    Serial3.println("||||||||||||||||||||||");
    Serial3.println("1 - Card Learning \n");
    Serial3.println("2 - Card Listing \n");
    Serial3.println("3 - Local EEPROM Delete \n");
    Serial3.println("4 - Local EEPROM Read Out All \n");
    Serial3.println("5 - Access Control \n");
    Serial3.println("7 - Card Deleting \n");
    Serial3.println("8 - Card Updating");
    Serial3.println("||||||||||||||||||||||\n");
    main_menu = 0;
    input_a = 0;
    }
      input_a = Serial3.read();
      //input_a = '5';
      if (input_a == '1')
      {
        CardLearningV3();
      }
      else if(input_a == '2')
      {
        //CardReadOut();
        //CardReadOutMySQL();
        NMC = true;
        CardReadOutMySQLv2();
        if(CardNumberFromDb != 0)
        {
          Serial3.print("Cards from DB are: "); Serial3.println(CardNumberFromDb);
        }
        CardNumberFromDb = 0;
      }
      else if(input_a == '3')
      {
        EEPROMDELETE();
      }
      else if(input_a == '4')
      {
        EEPROMREADOUTALL();
      }
      else if(input_a == '5' || logging)
      {
        logging = true;
        Serial3.println("Waiting for incoming data...");
        readfromoutsideLoRa();
      }
      else if(input_a == '7')
      {
        CardDeleting();
      }else if(input_a == '8')
      {
        CardUpdateToDB();
      }
  }
}
void CardLearningV2()
{
do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    Serial3.println("-------------------------------------------------------------------------------");
    Serial3.print("Kerem a""(z) "); Serial3.print(kartyadb); Serial3.print(". taget/kartyat. \n");
    Serial3.println("A visszalepeshez ird be a 6-ost!");
    //Scan RFID UID
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //Serial3.println("1");
    while(success == 0 && tag.toInt() == 0)
    {
      //Serial3.println("2");
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
      ///////////////////////////////////////
      ///////////////////////////////////////
      tag = myRfid.getTag();
      ///////////////////////////////////////
      //////////////////////////////////////
      input_a = Serial3.read();
      //Serial3.println("Waiting for Card...");
      if (input_a == '6')
      {
        main_menu = 1;
        input_a = 0;
        loop();
        return;
      }
    }
     ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////  

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
      if(tag.toInt() != 0)
      {
        unsigned int TagLength = tag.length();
        //Serial3.print("Current TagLength: "); Serial3.println(TagLength);
        while (TagLength < 16)
        {
          for (iRFID = 0; iRFID < TagLength; iRFID++)
          {
            DataString = tag.substring(c, b);
            IntegerDataStringArray[iRFID] = DataString.toInt();
            b++;
            c++;
          }
          iRFID++;
          while (TagLength < 16)
          {
            IntegerDataStringArray[iRFID] = RFIDID;
            TagLength++;
            iRFID++;
            RFIDID--;
          }
        }
        if (TagLength == 16)
        {
          //Serial3.print("Modified Length: "); Serial3.println(TagLength);
          for (unsigned int u = 0; u < TagLength; u++)
          {
            //Serial3.print(IntegerDataStringArray[u]);
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
        b = 1;c = 0;iRFID = 0;RFIDID = 14;
        //Serial3.print("TAG ID: ");
        for (int p = 0; p < 4; p++)
        {
          //Serial3.print("0x");Serial3.print(uid[p], HEX);Serial3.print(" ");
          uidLength++;
        }
      }
      ///////////////////////////////////////////////
      /////////////////////////////////////////////
    
    if (success > 0)
    {
      RFIDControlByte = 1;
      success = 0;
    }
    //////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    
    else if (tag.toInt() != 0)
    {
      RFIDControlByte = 2;
      /////////////////////////////
      //////////////////////////////
      //tag.toInt() = 0;

      //Itt csereltem
      //tag = "";
      tag.remove(0, tag.length());
      
      /////////////////////////////
      ////////////////////////////////
    }
    /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    if(RFIDControlByte == 1 || RFIDControlByte == 2)
    {  
      Serial3.println("A taget/kartyat beolvastuk! \n");
       Serial3.print ("A(z) "); Serial3.print(kartyadb); Serial3.print(". TAG/KARTYA UID:"); 
      if (RFIDControlByte == 1)
      {
      nfc.PrintHex(uid, uidLength);
      }
      else if (RFIDControlByte == 2)
      {
      for (int p = 0; p < 4; p++)
        {
        Serial3.print("0x");Serial3.print(uid[p], HEX);Serial3.print(" ");
        }
      Serial3.println();
      }
      //Leellenorizzuk, hogy mar fel van-e veve ez a UID
      byte value;
      int ID = 0;
      carddb = kartyadb * uidLength;
      address = 0;
      for (int i = 0; i < carddb; i++)
      {
        value = EEPROM.read(address);
        if(value == uid[ID])
        {
          //Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
          ID++, address++;
          //Serial3.println("Find the"); Serial3.print(ID); Serial3.print(" byte.");
          value = EEPROM.read(address);
          if(value == uid[ID])
          {
            //Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
            ID++, address++;
            value = EEPROM.read(address);
            if(value == uid[ID])
            {
              //Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
              ID++, address++;
              value = EEPROM.read(address);
              if(value == uid[ID])
              {
                //Serial3.print("A UID "); Serial3.print(ID); Serial3.print(". "); Serial3.print("byte-ja benne van az EEPROM "); Serial3.print(address);Serial3.print(". "); Serial3.print("cimeben! \n");
                ID++, address++;
                value = EEPROM.read(address);
                Serial3.println("Ez a kartya/tag mar fel lett veve! Kerlek valasz egy masikat!");
                Serial3.println("A visszalepeshez ird be a 6-ost!");
                delay(5000);
                RFIDControlByte = 0;
                for (o = 0; o < uidLength; o++)
                {
                  uid[o] = 0;
                }
                for (unsigned int u; u < 16; u++)
                {
                  IntegerDataStringArray[u] = 0;
                }
                TagLength = 0;
                ID = 0; address = 0;
                input_a = 0;
                //Itt csereltem
                //tag = "";
                tag.remove(0, tag.length());
                uidLength = 0;
                CardLearningV2();
                return;
              }
              address++;
            }
            address++;
          }
          address++;
        }
        address++;
      }
      if (uidLength == 4)
      {
        Serial3.println("Ez egy Mifare Classic kartya 4 byte UID vagy RFID TAG.");
        //EEPROM WRITE SCANNED RFID UID
        eeAddress = EEPROM.read(eeAddresseeprom);
        EEPROM.put(eeAddress, uid);
        eepromcim = EEPROM.read(eepromcimeeprom);
        Serial3.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial3.print(eeAddress); Serial3.print(". bytetol kezdve! \n");
        eeAddress=eeAddress+5;
        EEPROM.put(eeAddresseeprom,eeAddress);
        eepromcim=eepromcim+5;
        EEPROM.write(eepromcimeeprom,eepromcim);
        //delay(2000);
        kartyadb++;
        EEPROM.put(kartyadbeeprom, kartyadb);
        delay(2000);
        Serial3.println("------------------------------------------------------------------------------- \n");
      }
      else if (uidLength == 7)
      {
        Serial3.println("Ez egy Mifare Ultralight kartya 7 byte UID jelenleg nem támogatott!");
        CardLearningV2();
        //EEPROM WRITE SCANNED RFID UID
        EEPROM.put(eeAddress, uid);
        eeAddress=eeAddress+8;
        EEPROM.put(eeAddresseeprom,eeAddress);
        Serial3.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial3.print(eepromcim); Serial3.print(". bittol kezdve! ");
        eepromcim = eepromcim+8;
        EEPROM.put(eepromcimeeprom,eepromcim);
        //delay(2000);      
        kartyadb++; 
        EEPROM.put(kartyadbeeprom, kartyadb);
        Serial3.println();
        Serial3.println();
        Serial3.println("A kovetkezo TAG/KARTYA felvetelehez kerlek nyomd meg az 1-es gombot!");
        Serial3.println("A fomenube valo visszajutashoz kerlek nyomd meg a 6-os gombot!");
      }
      else if (uidLength > 7)
      {
        Serial3.println("A KARTYA/TAG nem kompatibilis!");
        CardLearningV2();
      }
    RFIDControlByte = 0;
    //Itt csereltem
    //tag = "";
    tag.remove(0, tag.length());
    //Serial3.println(TagLength);
    //Serial3.println(uidLength);
    
    for (int o = 0; o < uidLength; o++)
      {
        uid[o] = 0;
      }
      for (int p2 = 0; p2 < 4; p2++)
      {
        uid[p2] = 0;
        uidLength = 0;
      }
      Serial3.println();
      for (unsigned int u; u < TagLength; u++)
      {
        IntegerDataStringArray[u] = 0;
      }
      TagLength = 0;
    }
    
  }while ( 11 != 10);
}
void CardDeleting()
{
  //Read out how many cards are learnt
  /*kartyadb = EEPROM.read(kartyadbeeprom);
  if((kartyadb - 1) == 0)
  {
    Serial3.println("We haven't learnt any card yet! \n");
    main_menu = 1;
    loop();
    return;
  }else
  Serial3.print("Available Card(s): "); Serial3.println(kartyadb - 1);
  Serial3.println("Card Deleting");
  Serial3.println("Please enter the UID of the card!");
  while(1!=2)
  {
    input_a = Serial3.read();
    if(input_a == '6')
    {
      break;
    }
    for(int i = 0; i < Serial3.available(); i++)
    {
      CardDeleteUserInput[i] = Serial3.read();
      CardDeleteUserInput[i+2] = '.';
    }
    
  }
  //CardDeleteUserInput;
  if(sizeof(CardDeleteUserInput) > 4)
  {
    for (int i = 0; i < sizeof(CardDeleteUserInput); i++)
    {
      CardDeleteUserInput[i] = 0;
    }
    Serial3.println("The UID length is too long!");
    Serial3.println("Max UID length is 4 byte!");
    CardDeleting();
    return;
  }*/
  
  Serial3.println("Card deleting.");
  Serial.println("148AD219,");
  String Card_Delete_ID = "";
  int Card_Delete_ID_INT = 0; 
  int Card_Num_bytes = 0;
  String Card_Act_Num = "";
  String Card_Data_Income = "";
  int Act_Card_data = 0;
  while(true)
  {
    
    while(Serial.available() > 0)
    {
      
      Card_Delete_ID = Serial.readString();
      Serial3.println(Card_Delete_ID);
      Card_Data_Income = Card_Delete_ID.substring(0, 12);
    }  
    if(Card_Data_Income == "129812TUZZ22")
    {
      Card_Delete_ID_INT = Card_Delete_ID.toInt();
      Serial3.print("Card Data length is: "); Serial3.println(Card_Delete_ID);
      Card_Num_bytes = Card_Delete_ID_INT - 13;
      Card_Act_Num = Card_Delete_ID.substring(13, 13 + Card_Num_bytes);
      Act_Card_data = Card_Act_Num.toInt();
      if(Act_Card_data == 0)
      {
        Serial3.println("No cards in DB");
      }else
      {
        Serial3.print("Actual card data number in DB: "); Serial3.println(Act_Card_data);
        CardNUmToLoRa[3] = 0;
        Serial1.write(CardNUmToLoRa, sizeof(CardNUmToLoRa));
        delay(15000);
        Serial1.write(EndOfCardDataFromDBInt, sizeof(EndOfCardDataFromDBInt));
      }
      main_menu = 1;
      
      Serial3.println("Waiting for log/action");
      readfromoutsideLoRa();
      //loop();
    }
  }
}
void CardLearningV3()
{
  SendTimeToNextion();
  NextionCurrentTime = millis();
  //Itt csereltem
  //MySQLCardDataToDB = "";
  MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
  Serial3.println("Welcome to CardLearning");
  Serial3.println("Please Enter FirstName!");
  //Itt csereltem
  //LastName = "";
  LastName.remove(0, LastName.length());
  while(LastName == "")
  {
    wdt_reset();
    SendTimeToNextion();
    NextionCurrentTime = millis();
    //Serial3.println("OHOHO");
    FirstName = Serial3.readString();
    //Serial.println(28);
    if (FirstName != "")
    {
      FirstNameBool = true;
      Serial3.println(FirstName);
      Serial3.println("Please Enter LastName!");
      while(LastName == "")
      {
        wdt_reset();
        SendTimeToNextion();
        NextionCurrentTime = millis();
        LastName = Serial3.readString();
        if(LastName != "")
        {
          LastNameBool = true;
          Serial3.print(FirstName); Serial3.print(" "); Serial3.println(LastName);
          Serial3.println("Are you sure?");
          Serial3.println("(y) - is yes, (n) is no");
          while(LastNameBool)
          {
            wdt_reset();
            SendTimeToNextion();
            NextionCurrentTime = millis();
            input_a = Serial3.read();
            //Serial.println(input_a, HEX);
            if((input_a == 'y') || (input_a == 'Y'))
              {
                //Insert Data To Pi MySQLDB
                MySQLCardDataToDBID = "2";
                MySQLCardDataToDB = MySQLCardDataToDBID + MySQLSeparator + FirstName + MySQLSeparator + LastName + MySQLSeparator;
                Serial3.print("Data to MYSQL without UID: "); Serial3.println(MySQLCardDataToDB);
                CardLearningName = true;
                //FirstName = "";
                FirstNameBool = false;
                //LastName = "";
                //Ez itterdekes...
                LastNameBool = "";
                
                if(CardLearningName)
                {
                  while(!CardLearningProcess)
                  {
                    wdt_reset();
                    SendTimeToNextion();
                    NextionCurrentTime = millis();
                    GetCardDataFromMySQLDB();
                    Serial3.print("Please give the "); Serial3.print(MySQLCardNumber); Serial3.println(". Card!");
                    CardLearningMaster();
                  }
                }
              }else if((input_a == 'n') || (input_a == 'N'))
              {
                Serial3.print("Name: "); Serial3.print(FirstName); Serial3.print(" "); Serial3.print(LastName); Serial3.println(" Deleted.");
                //Itt csereltem
                //FirstName = "";
                FirstName.remove(0, FirstName.length());
                FirstNameBool = false;
                //Itt csereltem
                //LastName = "";
                LastName.remove(0, LastName.length());
                LastNameBool = "";
                CardLearningV3();
              }           
          }
        }
      }
    }      
  }
}
void GetCardDataFromMySQLDB()
{
  MySQLConnectionCurrTimeOut = millis();
  MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
  MySQLCardNumber = 0;
  //Req Pi available cards
  Serial3.println("Sent Req Data to Pi");
  Serial.println("123,");
  while(true)
  {
    
    SendTimeToNextion();
    NextionCurrentTime = millis();
    //Serial.print("Counter is: "); Serial.println(Counter);
    MySQLConnectionCurrTimeOut = millis();
    //Ez az eredeti
    //MySQLCardNumbersReturn = Serial.readString();
    MySQLCardNumbersReturn = Serial.readStringUntil('\n');
    //Serial3.println(MySQLCardNumbersReturn);
    if(MySQLConnectionCurrTimeOut > MySQLConnectionPrevTimeOut + MySQLConnectionTimeOutInterval)
    {
      //Serial.print("Final Counter is: "); Serial.println(Counter);
      //Counter = 0;
      //Serial.print("Released Counter is: "); Serial.println(Counter);
      //Serial.print("CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
      MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
      //Serial.print("Released CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
      Serial3.println("Failed to Connect to DB.");
      //Itt csereltem
      //FirstName = "";
      FirstName.remove(0, FirstName.length());
      FirstNameBool = false;
      //Itt csereltem
      //LastName = "";
      LastName.remove(0, LastName.length());
      LastNameBool = "";

      //CardLearningV3();
      delay(3000);
      
      main_menu = 1;
      loop();
      //return;
    }
    if(MySQLCardNumbersReturn == "0")
      {
        //Serial.print("Final Counter is: "); Serial.println(Counter);
        //Counter = 0;
        //Serial.print("Released Counter is: "); Serial.println(Counter);
        //Serial.print("CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
        //Serial.print("Released CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        MySQLCardNumber = MySQLCardNumbersReturn.toInt();
        Serial3.print("Current Number of Cards are: ");Serial3.println(MySQLCardNumber);
        MySQLCardNumber = MySQLCardNumber + 1;
        Serial3.print("Please give the "); Serial3.print(MySQLCardNumber); Serial3.println(". Card!");
        //Serial3.print("Next card number is ");Serial3.println(MySQLCardNumber);
        CardLearningMaster();
        //while(true);
      }else if(MySQLCardNumbersReturn > "0")
      {
        //Serial.print("Final Counter is: "); Serial.println(Counter);
        //Counter = 0;
        //Serial.print("Released Counter is: "); Serial.println(Counter);
        //Serial.print("CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
        //Serial.print("Released CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        MySQLCardNumber = MySQLCardNumbersReturn.toInt();
        if(MySQLCardNumber == 63)
        {
          Serial3.print("Actual number of Cards in DB are: "); Serial3.print(MySQLCardNumber);
          Serial3.print(". Maximum card number achieved. Please delete one card to upload a new one!");
          Serial3.println("Waiting for log/action");
          readfromoutsideLoRa();
        }
        Serial3.println("Nullanal nagyobb szamot kaptam!");
        Serial3.print("Current Number of Cards are: ");Serial3.println(MySQLCardNumber);
        MySQLCardNumber = MySQLCardNumber + 1;
        Serial3.print("Please give the "); Serial3.print(MySQLCardNumber); Serial3.println(". Card!");
        //Serial3.print("Next card number is ");Serial3.println(MySQLCardNumber);
        CardLearningMaster();
        //while(true);
        /*Serial.print("Number of Cards: "); Serial.println(MySQLCardNumber - 1);
        Serial.print("Next Card is: "); Serial.println(MySQLCardNumber);*/
      }
      //Counter++;
      //delay(1000);
  }
}
void CardLearningMaster()
{
  wdt_reset();
  SendTimeToNextion();
  NextionCurrentTime = millis();
  for (unsigned int i = 0; i < sizeof(uid); i++)
      {
        
        uid[i] = 0;
      }
  uidLength = 0;
  //char NFCCARDBUFFER[8] ="";
  //ide kell a kartyaolvas
  Serial3.println("ittvagyok1");
  while(uid[0] == 0)
  {
    wdt_reset();
    SendTimeToNextion();
    NextionCurrentTime = millis();
    //delay(500);
    nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    tag = myRfid.getTag();
    delay(500);
    if(tag.toInt() < 0)
    {
      uid[0] = 1;
    }
  }
  if(tag.toInt() < 0)
  {
    uid[0] = 0;
  }
  //Sima Stringkent irjuk ki a taget
  Serial3.print("Original RFID TAG is: "); Serial3.println(tag);
  nfc.PrintHex(uid, uidLength);
  success = 0;
  Serial3.println("ittvagyok3");
  Serial3.println(uid[0], HEX);
  //If RFID
  if (tag.toInt() < 0)
  {
    Serial3.println("it's RFID tag");
    unsigned int TagLength = tag.length();
        //Serial.print("Current TagLength: "); Serial.println(TagLength);
        while (TagLength < 16)
        {
          wdt_reset();
          SendTimeToNextion();
          NextionCurrentTime = millis();
          for (iRFID = 0; iRFID < TagLength; iRFID++)
          {
            
            DataString = tag.substring(c, b);
            IntegerDataStringArray[iRFID] = DataString.toInt();
            b++;
            c++;
          }
          iRFID++;
          while (TagLength < 16)
          {
            wdt_reset();
            IntegerDataStringArray[iRFID] = RFIDID;
            TagLength++;
            iRFID++;
            RFIDID--;
          }
        }
        if (TagLength == 16)
        {
          //Serial.print("Modified Length: "); Serial.println(TagLength);
          for (unsigned int u = 0; u < TagLength; u++)
          {
            
            //Serial.print(IntegerDataStringArray[u]);
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
        b = 1;c = 0;iRFID = 0;RFIDID = 14;
        Serial3.print("TAG ID: "); Serial3.print("Normal: ");
        for (int p = 0; p < 4; p++)
        {
          
          //Serial.print("0x");
          Serial3.print(uid[p]);
          //Serial.print(" ");
          uidLength++;
        }
        Serial3.println();
    Serial3.println("Card From RFID Reader...");
    for (int ichar = 0; ichar < 3; ichar++)
      {
        
        sprintf(uidchar,"%X", uid[ichar]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        uidchar[6] = '\0';
        Serial3.print(ichar+1);Serial3.print(" String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
      //the last byte change to String HEX
    sprintf(uidchar, "%X", uid[3]);
    uidString = uidchar;
    uidString += "-";
    //Itt is csereltem
    uidchar[6] = '\0';
    Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
    UidStringToMysql += uidString + uidStringend;
      //UidStringToMysql = "";
  }
  //NFC Card
  else if(tag = "000000" && uidLength != 0)
  {
    Serial3.println("Card From NFC Reader...");
    for (int ichar = 0; ichar < 3; ichar++)
      {
        
        sprintf(uidchar,"%X", uid[ichar]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        uidchar[6] = '\0';
        Serial3.print(ichar);Serial3.print(" String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
    //  UidStringToMysql += uidStringend;
    sprintf(uidchar, "%X", uid[3]);
    uidString = uidchar;
    uidString += "-";
    //Itt is csereltem
    uidchar[6] = '\0';
    Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
    UidStringToMysql += uidString + uidStringend;
  }
  
  /*Serial3.print("  UID Value: ");
  nfc.PrintHex(uid, uidLength);*/
  //Serial3.println();
  //CardData = uid[0];
  
  
  //Serial3.println(NFCCARDBUFFER);
  Serial3.print("String is: "); Serial3.println(UidStringToMysql);
  CardData = UidStringToMysql;
  //Itt is csereltem
  //UidStringToMysql = "";
  UidStringToMysql.remove(0, UidStringToMysql.length());
  //Serial3.print("Modified: "); Serial3.println(UidStringToMysql);
  for (unsigned int i = 0; i < sizeof(uid); i++)
  {
    
    uid[i] = 0;
  }
  uidLength = 0;
  //NFCCARDBUFFER[8] = "";
  MySQLCardDataToDB += CardData + MySQLSeparator;
  Serial3.print("Data to MYSQL: "); Serial3.println(MySQLCardDataToDB);
  Serial.println(MySQLCardDataToDB);
  //while(true);
  MySQLConnectionCurrTimeOut = millis();
  //{
    //Ez az eredeti
    //MySQLCardNumbersReturn = Serial.readString();
    MySQLCardNumbersReturn = Serial.readStringUntil('\n');
    if(MySQLCardNumbersReturn == "555")
    {
      MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
      Serial3.println("Please choose another card, it's already taken hit (y) or hit (n) to go back!");
      //Itt is csereltem
      //MySQLCardDataToDB = "";
      MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
      for (unsigned int i = 0; i < sizeof(uid); i++)
      {
        
        uid[i] = 0;
      }
      uidLength = 0;
      while(true)
      {
        wdt_reset();
        SendTimeToNextion();
        NextionCurrentTime = millis();        
        int CardAlRDYTaken2 = Serial3.read();
        if(CardAlRDYTaken2 == 'n' || CardAlRDYTaken2 == 'N')
        {
          uidLength = 0;
          //Itt is csereltem
          //MySQLCardDataToDB = "";
          MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
          //Itt is csereltem
          //FirstName = "";
          FirstName.remove(0, FirstName.length());
          //Itt is csereltem
          //LastName = "";
          LastName.remove(0, LastName.length());
          FirstNameBool = false;
          LastNameBool = false;
          CardLearningName = false;
          CardLearningProcess = false;          
          //CardLearningV3();
          main_menu = 1;
          //loop();
          NMC = true;
          CardReadOutMySQLv2();
          Serial3.println("Waiting for log/action");
          readfromoutsideLoRa();
          //return;
        }
        else if(CardAlRDYTaken2 == 'y' || CardAlRDYTaken2 == 'Y')
        {
          if(MySQLCardDataToDBID == "47")
          {
            MySQLCardDataToDB += MySQLCardDataToDBID + MySQLSeparator + UpFirstName + MySQLSeparator + UpLastName + MySQLSeparator;
            CardLearningMaster();
          }else if(MySQLCardDataToDBID == "48")
          {
            
            MySQLCardDataToDB += MySQLCardDataToDBID + MySQLSeparator + UuserID + MySQLSeparator;
            CardLearningMaster();
          }else
          {
            MySQLCardDataToDB += MySQLCardDataToDBID + MySQLSeparator + FirstName + MySQLSeparator + LastName + MySQLSeparator;
            CardLearningMaster();
          }
          
          
        }
      }
    }else if(MySQLCardNumbersReturn == "556")
    {
      MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
      Serial3.println("Card Saved to DB!");
      for (unsigned int i = 0; i < sizeof(uid); i++)
      {
        uid[i] = 0;
      }
      uidLength = 0;
      //Itt is csereltem
      //MySQLCardDataToDB = "";
      MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
      //Itt is csereltem
      //FirstName = "";
      FirstName.remove(0, FirstName.length());
      //Itt is csereltem
      //LastName = "";
      LastName.remove(0, LastName.length());
      FirstNameBool = false;
      LastNameBool = false;
      CardLearningName = false;
      CardLearningProcess = false;
      Serial3.print("Do you want to add one more Card (y)? If you dont, please hit (n)!");
      while(true)
      {
        wdt_reset();
        SendTimeToNextion();
        NextionCurrentTime = millis();
        CardChoice2 = Serial3.read();
        //Serial3.println(1);
        if(CardChoice2 == 'y' || CardChoice2 == 'Y')
        {
          if(MySQLCardDataToDBID == "47" || MySQLCardDataToDBID == "48")
          {
            
            CardUpdateToDB();
          }else
          {
            CardLearningV3();
          }
          
          
        }
        else if(CardChoice2 == 'n' || CardChoice2 == 'N')
        {
          uidLength = 0;
          //Itt is csereltem
          //MySQLCardDataToDB = "";
          MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
          //Itt is csereltem
          //FirstName = "";
          FirstName.remove(0, FirstName.length());
          //Itt is csereltem
          //LastName = "";
          LastName.remove(0, LastName.length());
          FirstNameBool = false;
          LastNameBool = false;
          CardLearningName = false;
          CardLearningProcess = false;
          main_menu = 1;
          //loop();
          NMC = true;
          CardReadOutMySQLv2();

          Serial3.println("Waiting for log/action");
          readfromoutsideLoRa();
          //return;
        }
      }
      
    }else
    {
      int HG = Serial.read();
      Serial3.print("Ez jott: "); Serial3.println(HG);
    }
    
  //}
  //CardLearningV3();  
}
void accesscontrolmysql()
{
  Serial3.println("na");
  //Serial3.println("Waiting fir incoming data...");
  Serial3.println("Carding at: ");
  //Year
        if (byteFromESP32[9] < 2100)
        {
          Serial3.print(20, DEC);Serial3.print(byteFromESP32[9], DEC);Serial3.print(".");
        }
        //month
        if (byteFromESP32[10] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[10], DEC);Serial3.print(".");
        //day
        if (byteFromESP32[11] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[11], DEC);Serial3.print(".");
        //dayofweek
          Serial3.print(" (");Serial3.print(byteFromESP32[12], DEC);Serial3.print(") ");
          //hour
        if (byteFromESP32[13] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[13], DEC);Serial3.print(":");
          //minutes
        if (byteFromESP32[14] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[14], DEC);Serial3.print(":");
        //seconds
        if (byteFromESP32[15] < 10)
        {
          Serial3.print(0, DEC);
        }
        Serial3.print(byteFromESP32[15], DEC);Serial3.println();
        //////
        //Release Data
        Serial3.println();
    while (true)
  {
    ReadedCard = false;
    input_a = Serial3.read();
    if(input_a == '6')
    {
      main_menu = 1;
      loop();

    }
    //char NFCCARDBUFFER[8] ="";

    //ide kell a kartyaolvas
    //Serial3.println("ittvagyok1");
    /*while(uid[0] == 0)
    {
      //delay(500);
      nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
      tag = myRfid.getTag();
      delay(500);
      if(tag.toInt() < 0)
      {
        uid[0] = 1;
      }
    }
    if(tag.toInt() < 0)
      {
        uid[0] = 0;
      }
    Serial3.print("RFID TAG is: "); Serial3.println(tag);
    nfc.PrintHex(uid, uidLength);
    success = 0;*/
    Serial3.println("ittvagyok4");
    Serial3.print("UID IS: "); Serial3.print(byteFromESP32[2], HEX); Serial3.print(byteFromESP32[3], HEX); Serial3.print(byteFromESP32[4], HEX); Serial3.print(byteFromESP32[5], HEX); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.println(byteFromESP32[8], HEX);
    /*if (tag.toInt() < 0)
    {
      Serial3.println("RFID");
      unsigned int TagLength = tag.length();
          //Serial.print("Current TagLength: "); Serial.println(TagLength);
          while (TagLength < 16)
          {
            for (iRFID = 0; iRFID < TagLength; iRFID++)
            {
              DataString = tag.substring(c, b);
              IntegerDataStringArray[iRFID] = DataString.toInt();
              b++;
              c++;
            }
            iRFID++;
            while (TagLength < 16)
            {
              IntegerDataStringArray[iRFID] = RFIDID;
              TagLength++;
              iRFID++;
              RFIDID--;
            }
          }
          if (TagLength == 16)
          {
            //Serial.print("Modified Length: "); Serial.println(TagLength);
            for (unsigned int u = 0; u < TagLength; u++)
            {
              //Serial.print(IntegerDataStringArray[u]);
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
          b = 1;c = 0;iRFID = 0;RFIDID = 14;
          //Serial.print("TAG ID: ");
          for (int p = 0; p < 4; p++)
          {
            //Serial.print("0x");Serial.print(uid[p], HEX);Serial.print(" ");
            uidLength++;
          }
    }*/
    //Itt is csereltem
    //UidStringToMysql = "";
    UidStringToMysql.remove(0, UidStringToMysql.length());
    if(byteFromESP32[0] == 255 && byteFromESP32[1] != 155)
    {
      Serial3.println();
      Serial3.println("Card From NFC Reader...");
      
      for (int ichar = 0; ichar < 3; ichar++)
      {
        sprintf(uidchar,"%X", byteFromESP32[ichar+2]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        //uidchar[6] = "";
        uidchar[6] = '\0';
        Serial3.print(ichar);Serial3.print("String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
      sprintf(uidchar, "%X", byteFromESP32[5]);
      uidString = uidchar;
      uidString += "-";
      //Itt is csereltem
      //uidchar[6] = "";
      uidchar[6] = '\0';
      Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
      UidStringToMysql += uidString + uidStringend;
      Serial3.print("String is: "); Serial3.println(UidStringToMysql);


      /*sprintf(NFCCARDBUFFER,"%X%X%X%X%X%X%X%X",byteFromESP32[2], 4095, byteFromESP32[3], 4095, byteFromESP32[4], 4095, byteFromESP32[5], 4095); //, byteFromESP32[6], byteFromESP32[7], byteFromESP32[8]);
      Serial3.print("Card Data After editing: ");
      Serial3.println(NFCCARDBUFFER);*/
      ReadedCard = true;
    }
    else if(byteFromESP32[0] == 255 && byteFromESP32[1] == 155)
    {
      Serial3.println("Card From RFID Reader...");
      for (int ichar = 0; ichar < 3; ichar++)
      {
        sprintf(uidchar,"%X", byteFromESP32[ichar+2]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        //uidchar[6] = "";
        uidchar[6] = '\0';
        Serial3.print(ichar);Serial3.print("String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
      sprintf(uidchar, "%X", byteFromESP32[5]);
      uidString = uidchar;
      uidString += "-";
      //Itt is csereltem
      //uidchar[6] = "";
      uidchar[6] = '\0';
      Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
      UidStringToMysql += uidString + uidStringend;
      Serial3.print("String is: "); Serial3.println(UidStringToMysql);





      /*sprintf(NFCCARDBUFFER,"%X%X%X%X%X%X%X%X",byteFromESP32[2], 4095, byteFromESP32[3], 4095, byteFromESP32[4], 4095, byteFromESP32[5], 4095); //byteFromESP32[6], byteFromESP32[7], byteFromESP32[8]);*/

      ReadedCard = true;
    }
    //Release Tag
    //Itt is csereltem
    //tag = "";
    tag.remove(0, tag.length());
    for (unsigned int i = 0; i < sizeof(byteFromESP32); i++)
      {
        byteFromESP32[i] = 0;
      }
    uidLength = 0;
    //Itt is csereltem
    //CardData = "";
    CardData.remove(0, CardData.length());
    CardData = UidStringToMysql;
    //Itt is csereltem
    //UidStringToMysql = "";
    UidStringToMysql.remove(0, UidStringToMysql.length());
    Serial3.println(CardData);
    //Itt is csereltem
    //MySQLCardDataToDB = "";
    MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
    if(CardData != "")
    {
      MySQLCardDataToDB += MySQLAccessControlIdentifier + MySQLSeparator + CardData + MySQLSeparator;
      Serial3.print("Data to MYSQL: "); Serial3.println(MySQLCardDataToDB);
      Serial3.println(MySQLCardDataToDB);
      Serial.println(MySQLCardDataToDB);
    }
    //while(1);
    //Itt is csereltem
    //MySQLCardNumbersReturn = "";
    MySQLCardNumbersReturn.remove(0, MySQLCardNumbersReturn.length());
    MySQLConnectionCurrTimeOut = millis();
    MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
    while(ReadedCard)
    {
      Serial3.println("1x");
      MySQLConnectionCurrTimeOut = millis();
      //Ez az eredeti
      //MySQLCardNumbersReturn = Serial.readString();
      MySQLCardNumbersReturn = Serial.readStringUntil('\n');
      if(MySQLConnectionCurrTimeOut > MySQLConnectionPrevTimeOut + MySQLConnectionTimeOutInterval)
      {
        //Serial.print("Final Counter is: "); Serial.println(Counter);
        //Counter = 0;
        //Serial.print("Released Counter is: "); Serial.println(Counter);
        //Serial.print("CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
        //Serial.print("Released CurrentPrevTimeout Counter is: "); Serial.println(MySQLConnectionPrevTimeOut);
        Serial3.println("Failed to Connect to DB.");
        //CardLearningV3();
        delay(3000);
        ReadedCard = false;
        main_menu = 1;
        loop();
        //return;
      }
      else if(MySQLCardNumbersReturn == "553")
      {
        Serial3.println("No Cards in DB!");
        ReadedCard = false;
        return;
      }
      else if(MySQLCardNumbersReturn == "554")
      {
        Serial3.println("Passed!");
        Serial1.flush();
        Status = 12;
        Serial1.write(Status);
        ReadedCard = false;
        Serial3.println("ITttttt");
        return;
      }else if(MySQLCardNumbersReturn == "552")
      {
        Serial3.println("Rejected!");
        Serial1.flush();
        Status = 11;
        Serial1.write(Status);
        Serial3.print("Status byte is: "); Serial3.println(Status);
        
        ReadedCard = false;
        return;      
      }
    }
    return;
  }


}
/*
void CardReadOutMySQL()
{
  Serial3.println("Req from DB");
  MySQLCardReadOutIndentifier += MySQLSeparator;
  Serial.println(MySQLCardReadOutIndentifier);
  MySQLCardReadOutIndentifier = "4";
  while(true)
  {
    input_a = Serial3.read();
    if(input_a == '6')
    {
      input_a = 0;
      main_menu = 1;
      loop();
      return;
    }
    //Serial3.println("itttttt");
    CardDataFromDB = "";
    while (Serial.available() > 0)
    {
      CardDataFromDB = Serial.readString();
      Serial3.print("Card UID is: "); Serial3.println(CardDataFromDB);
      //CardDataFromDB = "";
    }
    //Count characters
    if (CardDataFromDB != "")
    {
      int UIDSCOUNT = 0;
      int UIDPrevEnd = 0;
      String UIDEnddetection = "*";
      String UIDBytesdetection = "-";
      String UIDSTRDBLength = "";
      int CHR = 0;
      int z = 0;
      uint8_t UIDEndPositions[] = {};
      int UIDCCFFBB2 = 0;
      uint8_t UIDCharacterFromDBBytes1[] = {};
      String UIDCharacterFromDBBytesConTainer = "";
      char UIDSTRTOCHRCOPY[] = "";
      char UIDSTRTOCHRCOPYLength[] = "";
      String UIDBytes = "";
      char CHRUID[] = "";
      int p = 0;
      int UIDINTDBLength = 0;
      byte UIDCCFFBB = 0;
      UIDSTRDBLength = CardDataFromDB.length();
      UIDINTDBLength = UIDSTRDBLength.toInt();
      Serial3.print("DB UID characterLength is: "); Serial3.println(UIDINTDBLength);
      //CardDataFromDB.substring(0);
      //CardDataFromDB = "";
      for (int i = 0; i < UIDINTDBLength+1; i++)
      {
        String UIDCharacterFromDB = CardDataFromDB.substring(i,i+1);
        
        
        if (UIDCharacterFromDB == UIDEnddetection)
        {
          UIDSCOUNT++;

          icount = i;
          //z++;
          //Serial3.println(icount);
          //while(1);

          for(;CHR < icount; CHR++)
          {
            //Serial3.println(CHR);
            z++;
            String UIDCharacterFromDBBytes = CardDataFromDB.substring(CHR,CHR+1);
            if(UIDCharacterFromDBBytes != "-")
            {
              UIDCharacterFromDBBytesConTainer += UIDCharacterFromDBBytes;

              //char charUID[3];
              //UIDCharacterFromDBBytesConTainer.toLowerCase();
              //UIDCHRT = UIDCharacterFromDBBytesConTainer.toInt();

              int UIDByteLength = UIDCharacterFromDBBytesConTainer.length();
              UIDSTRTOCHRCOPYLength[UIDByteLength];
              UIDCharacterFromDBBytesConTainer.toCharArray(UIDSTRTOCHRCOPY, UIDSTRTOCHRCOPYLength);


              Serial3.print("String data is: ");Serial3.println(UIDCharacterFromDBBytesConTainer);
              Serial3.print("char data is: ");Serial3.println(UIDSTRTOCHRCOPY);
              //strtol()
              sscanf(CHRUID, "%X", UIDCharacterFromDBBytesConTainer);
              //strtok()
              Serial3.print("Int is: "); Serial3.println(CHRUID);
              if(UIDCharacterFromDBBytes == "A" || UIDCharacterFromDBBytes == "B" || UIDCharacterFromDBBytes == "C" || UIDCharacterFromDBBytes == "D" || UIDCharacterFromDBBytes == "E" || UIDCharacterFromDBBytes == "F" )
              {
                sscanf(CHRUID, "%X", UIDSTRTOCHRCOPY);       
                Serial3.print("sscanf data is: ");Serial3.println(CHRUID);
              }*/
              //Serial3.println(charUID, DEC);
              //while(1);                       
            //}            
            //else if(UIDCharacterFromDBBytes == "-")
            //{
              
              
              /*int UIDByteLength = UIDCharacterFromDBBytesConTainer.length();
              UIDSTRTOCHRCOPYLength[UIDByteLength];
              UIDCharacterFromDBBytesConTainer.toCharArray(UIDSTRTOCHRCOPY, UIDSTRTOCHRCOPYLength);*/

              //sscanf(CHRUID, "%X%X");
              
              
              
              
              //UIDCharacterFromDBBytes1[UIDCCFFBB] = atoi(UIDSTRTOCHRCOPY);
              //Serial3.print(UIDCCFFBB+1); Serial3.print(" byte is: "); Serial3.println(UIDCharacterFromDBBytes1[UIDCCFFBB], HEX);
              
              
              //Serial3.print(UIDCCFFBB2);Serial3.print(" . bytes: "); Serial3.println(UIDCharacterFromDBBytesConTainer);
              




              //Serial3.print(UIDCCFFBB2);Serial3.print(" . hex bytes: "); Serial3.println(UIDCharacterFromDBBytes1[UIDCCFFBB], HEX);
              //Serial3.print(UIDCCFFBB2);Serial3.print(" . bytes: "); Serial3.println(UIDCharacterFromDBBytes1[UIDCCFFBB], HEX);
              
              
              //UIDCCFFBB2++;
              
            /*  UIDCCFFBB++;
              UIDCharacterFromDBBytesConTainer = "";
              UIDCharacterFromDBBytes = "";
              //UIDCCFFBB2 = 0;
              if(UIDCCFFBB == 3)
              {
                UIDCCFFBB = 0;
              }
            }*/
            /*if(UIDCCFFBB2 > 3)
            {
              //Serial3.print(UIDCCFFBB2);Serial3.print(" . bytes0: "); Serial3.println(UIDCharacterFromDBBytesConTainer);
              UIDCCFFBB2 = 0;
              UIDCCFFBB = 0;
            }*/
          //}
          //Serial3.println(CHR);
          //while(1);


          /*CHR = icount+1;
          Serial3.print("CHR after editing: ");Serial3.println(CHR);
          Serial3.print("Cycle is: "); Serial3.println(z);
          z = 0;
          UIDPrevEnd = icount;*/
          /*for(int UIDDBcardsCount = 0; UIDDBcardsCount < icount; UIDDBcardsCount++)
            {
              //z++;
            


            }*/







          //UIDGenerator();
          //Serial3.print("UID finish bytes are: "); Serial3.println(icount);
        //}

      //}




      /*CardDataFromDB = "";
      
      Serial3.print(UIDSCOUNT); Serial3.println(" UIDS got from DB.");
      //Serial3.println(z);

      UIDSCOUNT = 0;
    }
  }

}*/

void CardReadOutMySQLv2()
{
  wdt_reset();
  SendTimeToNextion();
  NextionCurrentTime = millis();  
  Serial3.println("Req from DB");
  MySQLCardReadOutIndentifier = "4";
  MySQLCardReadOutIndentifier += MySQLSeparator;
  
  //Send DB request
  Serial.println(MySQLCardReadOutIndentifier);
  Serial3.println(MySQLCardReadOutIndentifier);
  String MysqlCardDatafromDB;
  MysqlCardDatafromDB.remove(0, MysqlCardDatafromDB.length());
  String DatalengthStr = "";
  int Datalength = 0;
  boolean CardsFromDB = true;
  String UIDString = "";
  CardNumberFromDb = 0;
  int StarPlaceString = 0;
  String UIDBytesStr1 = "";
  String UIDBytesStr2 = "";
  String UIDBytesStr3 = "";
  String UIDBytesStr4 = "";
  String UIDBytesAdd = "";
  String CardNumbersStore = "";
  int PreviousStarPlaceString = 0;
  int CHR = 0;
  int z = 0;
  char StringtoCharUID1[4];
  char StringtoCharUID2[4];
  char StringtoCharUID3[4];
  char StringtoCharUID4[4];
  //int val = 0;
  uint8_t CardFromDBIntarray[6] = {255, 254, 0, 0 , 0, 0};
  //char EndOfCardDataFromDB[3] = "";
  String LastCardDatas  = "";
  //uint8_t CardNUmToLoRa[5] = {42, 42, 45, val, 42};
  while(CardsFromDB)
  {
    
    SendTimeToNextion();
    NextionCurrentTime = millis();
    input_a = Serial3.read();
    if (input_a == '6')
    {
      main_menu = 1;
      input_a = 0;
      loop();
      return;
    }
    //int ia = 0;
    //delay(25);
    //long CarddataNumFromDB[3] = {0, 0, 0};
    while(Serial.available() > 0)
    {
      wdt_reset();
      SendTimeToNextion();
      NextionCurrentTime = millis();
      MysqlCardDatafromDB = Serial.readStringUntil('\n');
      Serial3.print("Data From DB: "); Serial3.println(MysqlCardDatafromDB);
      DatalengthStr = MysqlCardDatafromDB.length();
      Datalength = DatalengthStr.toInt();
      //Itt is csereltem
      //DatalengthStr = "";
      DatalengthStr.remove(0, DatalengthStr.length());
      Serial3.print("Data Length from DB is: "); Serial3.println(Datalength);
      //To Card numbers
      if(MysqlCardDatafromDB == "553")
      {
        NMC = false;
      }
      if (Datalength > 0 && NMC)
      {
        Serial3.println("Ide jovokasdaasf!!!");
        for (unsigned int u = 0; u < Datalength+1; u++)
        {
          SendTimeToNextion();
          NextionCurrentTime = millis();
          UIDString = MysqlCardDatafromDB.substring(u, u+1);
          if(UIDString != "*")
          {
            CardNumbersStore += MysqlCardDatafromDB.substring(u, u+1);
          }else if(UIDString == "*")
          {
            CardNumbersStore += MysqlCardDatafromDB.substring(u, u+1);
            String CNS = CardNumbersStore;
            CNS.remove(CNS.length()-1);
            String CNSREMOVE = CNS;
            char NumOfCardsFromDB[6] = "\0";
            //CNSREMOVE.remove(0,1);
            Serial3.print("Number of Cards are: "); Serial3.println(CNSREMOVE);
            u = Datalength+1;
            //vagy itt átírom akkora számra ahánykártya van és write-al küldöm el.
            //Serial1.print(CardNumbersStore);
            //Itt is csereltem
            //CardNumbersStore = "";
            CardNumbersStore.remove(0, CardNumbersStore.length());

            //NMC = false;

            CNSREMOVE.toCharArray(NumOfCardsFromDB, 6);
            val = StrToHex(NumOfCardsFromDB);
            CardNUmToLoRa[3] = val;
            Serial3.print("Number of card in hex: "); Serial3.println(val, HEX);
            Serial1.write(CardNUmToLoRa, sizeof(CardNUmToLoRa));
          }
        }
        NMC=false;        
      }else if(MysqlCardDatafromDB == "553")
      {
        Serial3.println("No card to fetch from DB!");
        Serial3.print(EmptyDBtoGateController[0], DEC); Serial3.print(" ");
        Serial3.print(EmptyDBtoGateController[1], DEC); Serial3.print(" ");
        Serial3.print(EmptyDBtoGateController[2], DEC); Serial3.print(" ");
        Serial.println(" ");
        Serial1.write(EmptyDBtoGateController, sizeof(EmptyDBtoGateController));
        //Serial1.write(RetryCardDataDownloadCallBackPackage, sizeof(RetryCardDataDownloadCallBackPackage));
        Serial3.println("Waiting for log/action");
        readfromoutsideLoRa();
      }
    }
    if(Datalength > 0 && !NMC)
    {
      Serial3.println("---------------");
      for(int i = 3; i < Datalength+1; i++)
      {
        
        SendTimeToNextion();
        NextionCurrentTime = millis();
        UIDString = MysqlCardDatafromDB.substring(i, i+1);
        if(UIDString == "*")
        {
          CardNumberFromDb++;
          StarPlaceString = i;
          //Serial3.print("* is: "); Serial3.println(StarPlaceString);
          //while(1);
          CHR = PreviousStarPlaceString;
          //Serial3.print("CHR now is: "); Serial3.println(CHR);
          for(; CHR < StarPlaceString; CHR++)
          {
            
            SendTimeToNextion();
            NextionCurrentTime = millis();
            //Serial3.print("CHR: "); Serial3.println(CHR);
            UIDBytesAdd = MysqlCardDatafromDB.substring(CHR, CHR+1);
            if(UIDBytesAdd != "-")
            {
              UIDBytesStr1 += UIDBytesAdd;
              //Itt is csereltem
              //UIDBytesAdd = "";
              UIDBytesAdd.remove(0, UIDBytesAdd.length());
              //Serial3.println("Megint itt...1");
            }else if(UIDBytesAdd == "-")
            {
              z++;
              Serial3.print(z);Serial3.print(". Data is: ");Serial3.println(UIDBytesStr1);
              CHR++;
              //Serial3.println("Megint itt...1.1");
              for(; CHR < StarPlaceString; CHR++)
              {
                
                SendTimeToNextion();
                NextionCurrentTime = millis();
                //Serial3.print("CHR: "); Serial3.println(CHR);
                UIDBytesAdd = MysqlCardDatafromDB.substring(CHR, CHR+1);
                if(UIDBytesAdd != "-")
                {
                  UIDBytesStr2 += UIDBytesAdd;
                  //Itt is csereltem
                  //UIDBytesAdd = "";
                  UIDBytesAdd.remove(0, UIDBytesAdd.length());
                  //Serial3.println("Megint itt...2");
                }else if(UIDBytesAdd == "-")
                {
                  z++;
                  Serial3.print(z);Serial3.print(". Data is: ");Serial3.println(UIDBytesStr2);
                  CHR++;
                  //Serial3.println("Megint itt...2.2");
                  for(; CHR < StarPlaceString; CHR++)
                  {
                    
                    SendTimeToNextion();
                    NextionCurrentTime = millis();
                    //Serial3.print("CHR: "); Serial3.println(CHR);
                    UIDBytesAdd = MysqlCardDatafromDB.substring(CHR, CHR+1);
                    if(UIDBytesAdd != "-")
                    {
                      UIDBytesStr3 += UIDBytesAdd;
                      //Itt is csereltem
                      //UIDBytesAdd = "";
                      UIDBytesAdd.remove(0, UIDBytesAdd.length());
                      //Serial3.println("Megint itt...3");
                    }else if(UIDBytesAdd == "-")
                    {
                      z++;
                      Serial3.print(z);Serial3.print(". Data is: ");Serial3.println(UIDBytesStr3);
                      CHR++;
                      //Serial3.println("Megint itt...3.3");
                      for(; CHR < StarPlaceString; CHR++)
                      {
                        
                        SendTimeToNextion();
                        NextionCurrentTime = millis();
                        //Serial3.print("CHR: "); Serial3.println(CHR);
                        UIDBytesAdd = MysqlCardDatafromDB.substring(CHR, CHR+1);
                        if(UIDBytesAdd != "-")
                        {
                          UIDBytesStr4 += UIDBytesAdd;
                          //Itt is csereltem
                          //UIDBytesAdd = "";
                          UIDBytesAdd.remove(0, UIDBytesAdd.length());
                          //Serial3.println("Megint itt...4");
                        }else if(UIDBytesAdd == "-")
                        {
                          z++;
                          Serial3.print(z);Serial3.print(". Data is: ");Serial3.println(UIDBytesStr4);
                          CHR++;
                          //Serial3.println("Megint itt...4.4");
                          for(; CHR < StarPlaceString+1; CHR++)
                          {
                            wdt_reset(); 
                            SendTimeToNextion();
                            NextionCurrentTime = millis();
                            //Serial3.print("Last CHR: "); Serial3.println(CHR);
                            UIDBytesAdd = MysqlCardDatafromDB.substring(CHR, CHR+1);
                            
                            if(UIDBytesAdd == "*")
                            {
                              SendTimeToNextion();
                              NextionCurrentTime = millis();
                              delay(450);
                              SendTimeToNextion();
                              
                              NextionCurrentTime = millis();
                              Serial3.print("Last Character is: "); Serial3.println(UIDBytesAdd);
                              Serial3.print("CHR is before editing: "); Serial3.println(CHR);
                              PreviousStarPlaceString = StarPlaceString + 1;
                              Serial3.print("CHR is after editing: "); Serial3.println(CHR);
                              z = 0;
                              Serial3.print("1st String: "); Serial3.println(UIDBytesStr1);
                              UIDBytesStr1.toCharArray(StringtoCharUID1, 4);
                              Serial3.print("1st char: "); Serial3.println(StringtoCharUID1);

                              Serial3.print("2nd String: "); Serial3.println(UIDBytesStr2);
                              UIDBytesStr2.toCharArray(StringtoCharUID2, 4);
                              Serial3.print("2nd char: "); Serial3.println(StringtoCharUID2);

                              Serial3.print("3rd String:"); Serial3.println(UIDBytesStr3);
                              UIDBytesStr3.toCharArray(StringtoCharUID3, 4);
                              Serial3.print("3rd char: "); Serial3.println(StringtoCharUID3);

                              Serial3.print("4th String:"); Serial3.println(UIDBytesStr4);
                              UIDBytesStr4.toCharArray(StringtoCharUID4, 4);
                              Serial3.print("4th char: "); Serial3.println(StringtoCharUID4);

                              int CDBI = 2;
                              val = StrToHex(StringtoCharUID1);
                              CardFromDBIntarray[CDBI] = val;
                              CDBI++;
                              Serial3.print(CDBI-2); Serial3.print(". char to int: "); Serial3.println(CardFromDBIntarray[CDBI-1], HEX);

                              val = StrToHex(StringtoCharUID2);
                              CardFromDBIntarray[CDBI] = val;
                              CDBI++;
                              Serial3.print(CDBI-2); Serial3.print(". char to int: "); Serial3.println(CardFromDBIntarray[CDBI-1], HEX);

                              val = StrToHex(StringtoCharUID3);
                              CardFromDBIntarray[CDBI] = val;
                              CDBI++;
                              Serial3.print(CDBI-2); Serial3.print(". char to int: "); Serial3.println(CardFromDBIntarray[CDBI-1], HEX);

                              val = StrToHex(StringtoCharUID4);
                              CardFromDBIntarray[CDBI] = val;
                              //CDBI++;
                              Serial3.print(CDBI-2); Serial3.print(". char to int: "); Serial3.println(CardFromDBIntarray[CDBI], HEX);
                              
                              Serial3.println("Data to Inside LoRa has started.");
                              Serial1.flush();
                              Serial1.write(CardFromDBIntarray, sizeof(CardFromDBIntarray));
                              

                              //Releases
                              //UIDBytesAdd = "";
                              //Itt is csereltem
                              //UIDBytesAdd = "";
                              UIDBytesAdd.remove(0, UIDBytesAdd.length());
                              //Itt is csereltem
                              //UIDBytesStr1 = "";
                              UIDBytesStr1.remove(0, UIDBytesStr1.length());
                              //Itt is csereltem
                              //UIDBytesStr2 = "";
                              UIDBytesStr2.remove(0, UIDBytesStr2.length());
                              //Itt is csereltem
                              //UIDBytesStr3 = "";
                              UIDBytesStr3.remove(0, UIDBytesStr3.length());
                              //Itt is csereltem
                              //UIDBytesStr4 = "";
                              UIDBytesStr4.remove(0, UIDBytesStr4.length());
                              CDBI = 2;
                              for (int i = 0; i < 3; i++)
                              {
                                //Itt is csereltem
                                StringtoCharUID1[i] = '\0';
                                //Itt is csereltem
                                StringtoCharUID2[i] = '\0';
                                //Itt is csereltem
                                StringtoCharUID3[i] = '\0';
                                //Itt is csereltem
                                StringtoCharUID4[i] = '\0';
                                CardFromDBIntarray[i+2] = 0;

                              }
                              
                              val = 0;
                              
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        wdt_reset();
        UIDString = Serial.readString();
        }
        if(UIDString == "@")
        {
          wdt_reset();
          //Itt is csereltem
          //LastCardDatas = "";
          LastCardDatas.remove(0, LastCardDatas.length());
          char LastCardDatChar[3] = "\0";
          int LCD = 0;
          Serial3.println("Utolso kartya adat is meg van");
          /*Serial3.println(i);
          Serial3.println(UIDString);
          Serial3.println(MysqlCardDatafromDB);*/
         LastCardDatas = MysqlCardDatafromDB.substring(i-2, i);
          //LastCardDatas += UIDString.substring(i-3, i-2);
          //LastCardDatas += UIDString.substring(i-2, i-1);
          //LastCardDatas += UIDString.substring(i-1, i);
          Serial3.println(LastCardDatas);
          LCD = LastCardDatas.toInt();
          Serial3.print("LCD is: ");Serial3.println(LCD);
          LastCardDatas.toCharArray(LastCardDatChar, 3);
          Serial3.println(LastCardDatChar[0]);
          Serial3.println(LastCardDatChar[1]);
          if(LastCardDatChar[0] == 63 && LastCardDatChar[1] == 63)
          {
            Serial3.println("Itten vagyok he");
            //uint8_t EndOfCardDataFromDBInt[3] = {63, 63, 64};
            Serial3.print("1st: "); Serial3.println(EndOfCardDataFromDBInt[0]);
            Serial3.print("2nd: "); Serial3.println(EndOfCardDataFromDBInt[1]);
            Serial3.print("3rd: "); Serial3.println(EndOfCardDataFromDBInt[2]);
            delay(50);
            Serial1.write(EndOfCardDataFromDBInt, sizeof(EndOfCardDataFromDBInt));            
            CardsFromDB = false;
            delay(500);
            wdt_reset();
            Serial1.write(RetryCardDataDownloadCallBackPackage, sizeof(RetryCardDataDownloadCallBackPackage));
            Serial3.println("Waiting for incoming Data");
            //Itt is csereltem
            //MysqlCardDatafromDB = "";
            MysqlCardDatafromDB.remove(0, MysqlCardDatafromDB.length());
            Serial3.println("Waiting for log/action");
            readfromoutsideLoRa();
            main_menu = 1;
            loop();            
          }
        }
      }
      StarPlaceString = 0;
      Datalength = 0;
    }
  }
}
int StrToHex(char str[])
{
  return (int) strtol(str, 0, 16);
}
void SendActionToMysql()
{
  unsigned int L_MySQLCardDataToDB = MySQLCardDataToDB.length();
  Serial3.print("Data Length in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardDataToDB);
  for(unsigned int i = 0; i < L_MySQLCardDataToDB; i++)
  {
    MySQLCardDataToDB.remove(i, 1000);
  }
  Serial3.print("Data in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardDataToDB);
  //Itt is csereltem
  //CardData = "";
  CardData.remove(0, CardData.length());
  //Itt is csereltem
  //MySQLCardDataToDB = "";
  MySQLCardDataToDB.remove(0, MySQLCardDataToDB.length());
  Serial3.println("Action saved to DB.");
  String ActionDataIdentifier = "1";
  String ActiondatatomysqlDB = "";
  String ActionType = "";
  String RTCtoDBStr = "";
  char RTCtoDB[7] = "\0";
  if((byteFromESP32[0] == 255 && byteFromESP32[1] == 255) || (byteFromESP32[0] == 255 && byteFromESP32[1] == 155))
  {
    Serial3.println("It's a Carding Action");
    //ActionType = "18";
    //it's 18 and it means opened
    //19 means unauthorized person
    ActionType += byteFromESP32[6];
    Serial3.print("Action type id is: "); Serial.println(ActionType);
    Serial3.print("UID IS: "); Serial3.print(byteFromESP32[2], HEX); Serial3.print(byteFromESP32[3], HEX); Serial3.print(byteFromESP32[4], HEX); Serial3.print(byteFromESP32[5], HEX); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.println(byteFromESP32[8], HEX);
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 255)
  {
    Serial3.println("Door opened action.");
    ActionType = "118";
    Serial3.print("Action IS: "); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.print(byteFromESP32[8], HEX); Serial3.print(byteFromESP32[9], HEX); Serial3.print(byteFromESP32[10], HEX); Serial3.print(byteFromESP32[11], HEX); Serial3.println(byteFromESP32[12], HEX);
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 254)
  {
    Serial3.println("Door closed action.");
    ActionType = "119";
    Serial3.print("Action IS: "); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.print(byteFromESP32[8], HEX); Serial3.print(byteFromESP32[9], HEX); Serial3.print(byteFromESP32[10], HEX); Serial3.print(byteFromESP32[11], HEX); Serial3.println(byteFromESP32[12], HEX);
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 20)
  {
    Serial3.println("The Door is opened Outside with Nextion.");
    ActionType = "20";
    Serial3.print("Action IS: "); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.print(byteFromESP32[8], HEX); Serial3.print(byteFromESP32[9], HEX); Serial3.print(byteFromESP32[10], HEX); Serial3.print(byteFromESP32[11], HEX); Serial3.println(byteFromESP32[12], HEX);
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 21)
  {
    Serial3.println("The Door is opened Outside with Button.");
    ActionType = "21";
    Serial3.print("Action IS: "); Serial3.print(byteFromESP32[6], HEX); Serial3.print(byteFromESP32[7], HEX); Serial3.print(byteFromESP32[8], HEX); Serial3.print(byteFromESP32[9], HEX); Serial3.print(byteFromESP32[10], HEX); Serial3.print(byteFromESP32[11], HEX); Serial3.println(byteFromESP32[12], HEX);
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 22)
  {
    ActionType = "22";
    Serial3.print("Retry Card Data Download Action at: "); Serial3.print("20"); Serial3.print(byteFromESP32[9], DEC); Serial3.print(" "); Serial3.print(byteFromESP32[10], DEC); Serial3.print(" "); Serial3.print(byteFromESP32[11], DEC); Serial3.print(" "); Serial3.print(" "); Serial3.print(byteFromESP32[13], DEC); Serial3.print(":"); Serial3.print(byteFromESP32[14], DEC); Serial3.print(":"); Serial3.println(byteFromESP32[15], DEC);
    //////
    ReadedCard = true;
    //////
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 23)
  {
    ActionType = "23";
    Serial3.println("Master User Signed in.");
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 24)
  {
    Serial3.println("Invailed code entered.");
    ActionType = "24";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 25)
  {
    Serial3.println("Master User Quit.");
    ActionType = "25";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 26)
  {
    Serial3.println("Power Cable is plugged in.");
    ActionType = "26";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 27)
  {
    Serial3.println("Power Cable is unplugged.");
    ActionType = "27";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 28)
  {
    Serial3.println("Powersupply DC Output power is sufficient.");
    ActionType = "28";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 29)
  {
    Serial3.println("Powersupply DC Output power is insufficient.");
    ActionType = "29";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 33)
  {
    Serial3.println("Door opened by SmartPhone App.");
    ActionType = "33";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 34)
  {
    Serial3.println("Retry Card Data Download Was Success.");
    ActionType = "34";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 35)
  {
    Serial3.println("Retry Card Data Download Was Success.");
    ActionType = "35";
  }else if(byteFromESP32[0] == 111 && byteFromESP32[1] == 111 && byteFromESP32[2] == 36)
  {
    Serial3.println("Retry Card Data Download Was Failed.");
    ActionType = "36";
  }
    //Itt is csereltem
    //UidStringToMysql = "";
    UidStringToMysql.remove(0, UidStringToMysql.length());
    if(byteFromESP32[0] == 255 && byteFromESP32[1] != 155)
    {
      Serial3.println();
      Serial3.println("Card From NFC Reader...");
      for (int ichar = 0; ichar < 3; ichar++)
      {
        sprintf(uidchar,"%X", byteFromESP32[ichar+2]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        uidchar[6] = '\0';
        Serial3.print(ichar);Serial3.print("String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
      sprintf(uidchar, "%X", byteFromESP32[5]);
      uidString = uidchar;
      uidString += "-";
      //Itt is csereltem
      uidchar[6] = '\0';
      Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
      UidStringToMysql += uidString;
      Serial3.print("String is: "); Serial3.println(UidStringToMysql);
      ReadedCard = true;
    }
    else if(byteFromESP32[0] == 255 && byteFromESP32[1] == 155)
    {
      Serial3.println("Card From RFID Reader...");
      for (int ichar = 0; ichar < 3; ichar++)
      {
        sprintf(uidchar,"%X", byteFromESP32[ichar+2]);
        uidString = uidchar;
        uidString += "-";
        //Itt is csereltem
        uidchar[6] = '\0';
        Serial3.print(ichar);Serial3.print("String is: "); Serial3.println(uidString);
        UidStringToMysql += uidString;
      }
      sprintf(uidchar, "%X", byteFromESP32[5]);
      uidString = uidchar;
      uidString += "-";
      //Itt is csereltem
      uidchar[6] = '\0';
      Serial3.print(4);Serial3.print(" String is: "); Serial3.println(uidString);
      UidStringToMysql += uidString;
      Serial3.print("String is: "); Serial3.println(UidStringToMysql);
      ReadedCard = true;
    }
      //Release Tag
      //Itt is csereltem
      //tag = "";
      tag.remove(0, tag.length());
      uidLength = 0;
      //Itt is csereltem
      //CardData = "";
      CardData.remove(0, CardData.length());
      CardData = UidStringToMysql;
      //Itt is csereltem
      //UidStringToMysql = "";
      UidStringToMysql.remove(0, UidStringToMysql.length());
      Serial3.print("Card Data is: "); Serial3.println(CardData);    
      //RTC Data to db
      MySQLCardDataToDB += ActionDataIdentifier + MySQLSeparator + CardData + MySQLSeparator;
      for (unsigned int i = 9; i < 16; i++)
      {
        
        sprintf(RTCtoDB, "%i", byteFromESP32[i]);
        RTCtoDBStr = RTCtoDB;
        //Itt is csereltem
        RTCtoDB[7] = '\0';
        Serial3.print(i);Serial3.print(" String is: "); Serial3.println(RTCtoDBStr);
        MySQLCardDataToDB += RTCtoDBStr;
      }
      MySQLCardDataToDB += MySQLSeparator + ActionType + MySQLSeparator;
      Serial3.print("Data to MYSQL: "); Serial3.println(MySQLCardDataToDB);
      Serial3.println(MySQLCardDataToDB);
      Serial.println(MySQLCardDataToDB);
    //MySQLCardNumbersReturn = "";
    //We should release MySQLCardNumbersReturn data
    unsigned int L_MySQLCardNumbersReturn = MySQLCardNumbersReturn.length();
    Serial3.print("Data Length in MySQLCardNumbersReturn: "); Serial3.println(L_MySQLCardNumbersReturn);
    
    Serial3.print("Data in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardNumbersReturn);
    for(unsigned int i = 0; i < L_MySQLCardNumbersReturn; i++)
    {
      MySQLCardNumbersReturn.remove(i, L_MySQLCardNumbersReturn);
    }
    MySQLConnectionCurrTimeOut = millis();
    MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
    for (unsigned int i = 0; i < sizeof(byteFromESP32); i++)
      {
        
        byteFromESP32[i] = 0;
      }
    while(ReadedCard)
    {
      NextionCurrentTime = millis();
      Serial3.println("1x");
      MySQLConnectionCurrTimeOut = millis();
      //Ez az eredeti
      //MySQLCardNumbersReturn = Serial.readString();
      MySQLCardNumbersReturn = Serial.readStringUntil('\n');
      Serial3.print("Return data is: "); Serial3.println(MySQLCardNumbersReturn);
      if((MySQLConnectionCurrTimeOut > MySQLConnectionPrevTimeOut + MySQLConnectionTimeOutInterval) && (MySQLCardNumbersReturn != "118" || MySQLCardNumbersReturn != "119"))
      {
        MySQLConnectionPrevTimeOut = MySQLConnectionCurrTimeOut;
        Serial3.println("Failed to Connect to DB.");
        delay(3000);
        L_MySQLCardNumbersReturn = MySQLCardNumbersReturn.length();
        Serial3.print("Data Length in MySQLCardNumbersReturn: "); Serial3.println(L_MySQLCardNumbersReturn);
        Serial3.print("Data in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardNumbersReturn);
        for(unsigned int i = 0; i < L_MySQLCardNumbersReturn; i++)
        {
          MySQLCardNumbersReturn.remove(i, L_MySQLCardNumbersReturn);
        }
        ReadedCard = false;
        main_menu = 1;
        loop();
      }else if(MySQLCardNumbersReturn == "22")
      {
        Serial3.println("Go back to fetch Card Data from DB.");
        Serial3.println("Request and Action Sent to DB.");
        Serial1.write(RetryCardDataDownloadStartedBackPackage, sizeof(RetryCardDataDownloadStartedBackPackage));
        //delay(500);
        //Release Data
        /*for (unsigned int z = 0; z < sizeof(byteFromESP32); z++)
        {
          byteFromESP32[z] = 0;
          //Serial3.print(byteFromESP32[z]);
        }
        Serial3.println();*/
        //////
        //ReadedCard = false;
        //NMC = true;
        //////
        L_MySQLCardNumbersReturn = MySQLCardNumbersReturn.length();
        Serial3.print("Data Length in MySQLCardNumbersReturn: "); Serial3.println(L_MySQLCardNumbersReturn);
        
        Serial3.print("Data in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardNumbersReturn);
        for(unsigned int i = 0; i < L_MySQLCardNumbersReturn; i++)
        {
          MySQLCardNumbersReturn.remove(i, L_MySQLCardNumbersReturn);
        }
        NMC=true;
        CardReadOutMySQLv2();
        
      }else
      {
        Serial3.print("Data in MySQLCardNumbersReturn: "); Serial3.println(MySQLCardNumbersReturn);
        L_MySQLCardNumbersReturn = MySQLCardNumbersReturn.length();
        Serial3.print("Data Length in MySQLCardNumbersReturn: "); Serial3.println(L_MySQLCardNumbersReturn);
        for(unsigned int i = 0; i < L_MySQLCardNumbersReturn; i++)
        {
          MySQLCardNumbersReturn.remove(i, L_MySQLCardNumbersReturn);
        }
        Serial3.println("Minden rendben volt...");
        Serial3.println("Waiting for log/action");
        readfromoutsideLoRa();
      }
      
    }

}
void ServerNTP()
{
  
  while(Serial.available() > 0)
  {
    PiNTP[iu] = Serial.read();
    /*Serial3.print(iu); Serial3.print(". ");
    Serial3.print(PiNTP[iu]); Serial3.print(" ");*/
    
    iu++;
  }
  if(PiNTP[0] == 118 && PiNTP[1] == 128 && PiNTP[2] == 255 && PiNTP[3] == 255 && PiNTP[4] == 255 && PiNTP[11] == 23 && PiNTP[12] == 123 && PiNTP[13] == 44 && PiNTP[14] == 234 && PiNTP[15] == 234 && PiNTP[16] == 234)
  {
    SendNTPServerTimeToNextion();
    Serial3.print("Current Date and Time from the server is: ");
    for (unsigned int u = 5; u < 11; u++)
    {
      Serial3.print(PiNTP[u]);
    }
    for (unsigned int i = 0; i < sizeof(PiNTP); i++)
    {
      PiNTP[i] = 0;
    }
    iu = 0;
    Serial3.println();
  }
}
void SendNTPServerTimeToNextion()
{
  uint8_t RTCHundred = 20;
  //Send Year
  Serial2.print("page0.n6.val=");
  Serial2.print(RTCHundred);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.print(RTCHundred);
  Serial2.print("page0.n7.val=");
  Serial2.print(PiNTP[5]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[5]);
  //Send Month
  Serial2.print("page0.n1.val=");
  Serial2.print(PiNTP[6]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[6])  ;
  //Send Day
  Serial2.print("page0.n2.val=");
  Serial2.print(PiNTP[7]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[7]);
  //Send Hour
  Serial2.print("page0.n3.val=");
  Serial2.print(PiNTP[8]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[8]);
  //Send Minute
  Serial2.print("page0.n4.val=");
  Serial2.print(PiNTP[9]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[9]);
  //Send Second
  Serial2.print("page0.n5.val=");
  Serial2.print(PiNTP[10]);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial3.println(PiNTP[10]);
  Serial3.println("Data has sent to Nextion.");
}
void TIMEAdjust()
{
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(12));
  //rtc.adjust((DateTime(2020, 7, 22, 18, 14, 0)) + TimeSpan(11));
  now = rtc.now();
}
void TimeNow()
{
  now = rtc.now();
    if (rtc.lostPower())
    {
    Serial3.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(11));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //now = rtc.now().unixtime();
    now = rtc.now();
  }
    Serial3.print("Current Date And Time: ");
    Serial3.print(now.year(), DEC);
    Serial3.print(".");
    if(now.month() < 10)
    {
        Serial3.print("0");
    }
    Serial3.print(now.month(), DEC);
    Serial3.print(".");
    if(now.day() < 10)
    {
        Serial3.print("0");
    }
    Serial3.print(now.day(), DEC);
    Serial3.print(". ");
    Serial3.print("(");
    Serial3.print(now.dayOfTheWeek(), DEC);
    Serial3.print(") ");
    if(now.hour() < 10)
    {
        Serial3.print("0");
    }
    Serial3.print(now.hour(), DEC);
    Serial3.print(":");
    if(now.minute() < 10)
    {
        Serial3.print("0");
    }
    Serial3.print(now.minute(), DEC);
    Serial3.print(":");
    if(now.second() < 10)
    {
        Serial3.print("0");
    }
    Serial3.println(now.second(), DEC);
    //delay(1000);
}
void TimeToNextion()
{
  now = rtc.now();
  //now = rtc.now();
  //Serial.println("Time Stamp: ");
  RTCData[0] = now.year()-2000; RTCData[1] = now.month(); RTCData[2] = now.day();
  RTCData[3] = now.dayOfTheWeek(); RTCData[4] = now.hour(); RTCData[5] = now.minute(); RTCData[6] = now.second();
  /*Serial3.print(RTCData[0]);
  Serial3.print(".");
  Serial3.print(RTCData[1]);
  Serial3.print(".");
  Serial3.print(RTCData[2]);
  Serial3.print(". ");
  Serial3.print(RTCData[4]);
  Serial3.print(":");
  Serial3.print(RTCData[5]);
  Serial3.print(":");
  Serial3.println(RTCData[6]);*/
  
}
void SendTimeToNextion()
{
  uint8_t RTCHundred = 20;
    if(NextionCurrentTime - NextionStartTime >= NextionTimeInterval)
    {
        NextionStartTime = NextionCurrentTime;
        TimeToNextion();
        //Send Year
        Serial2.print("page0.n6.val=");
        Serial2.print(RTCHundred);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.print("page0.n7.val=");
        Serial2.print(RTCData[0]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        //Send Month
        Serial2.print("page0.n1.val=");
        Serial2.print(RTCData[1]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        //Send Day
        Serial2.print("page0.n2.val=");
        Serial2.print(RTCData[2]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        //Send Hour
        Serial2.print("page0.n3.val=");
        Serial2.print(RTCData[4]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        //Send Minutes
        Serial2.print("page0.n4.val=");
        Serial2.print(RTCData[5]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
        //Send Seconds
        Serial2.print("page0.n5.val=");
        Serial2.print(RTCData[6]);
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.write(0xff);
    }
}
int CardUpdateToDB()
{
  Serial3.println("Please Choose Methodes:");
  Serial3.println("Methode 1: Update User's Card by name - type 1");
  Serial3.println("Methode 2: Update User's Card by User's ID - type 2");
  boolean /*UInputBool,*/ CardUpdateBool = true;
  
  //UpLastName, UpFirstName = "";
  boolean UpFirstNameBool, UpLastNameBool, updatetry;
  //unsigned int UInputLen;
  UpLastName.remove(0, UpLastName.length());
  UpFirstName.remove(0, UpFirstName.length());
  PrevUpdateCancel = millis();
  
  do
  {
    
    CardUpdateTimeCurrent = millis();
    CardUpdateTimePrev = millis();
    PrevUpdateCancel = millis();
    //if(Serial3.available() > 0)
    //{
     
      UInput = Serial3.read();
      if(UInput == '6')
      {
        
        UInput = 0;
        main_menu = 1;
        loop();
      }else if(UInput == '1')
      {
        UInput = 0;
        Serial3.println("Update User's Card by name SELECTED");
        Serial3.println("Please enter the first name!");
        MySQLCardDataUpdateID = "26";
        
        unsigned int UpLastNameLen = UpLastName.length();
        unsigned int UpFirstNameLen = UpFirstName.length();
        Serial3.print("FirstName Length is: "); Serial3.println(UpFirstName);
        Serial3.print("LastName Length is: "); Serial3.println(UpLastNameLen);
        Serial3.print("UpLastName data is: "); Serial3.println(UpLastName);
        while(UpLastNameLen == 0)
        {
          
          PrevUpdateCancel = millis();
          //Serial3.println("Haguz!");
          CardUpdateTimeCurrent = millis();
          CardUpdateTimePrev = millis();
          UpFirstName = Serial3.readString();
          UpFirstNameLen = UpFirstName.length();
          if(UpFirstNameLen != 0)
          {
            UpFirstNameBool = true;
            Serial3.println(UpFirstName);
            Serial3.println("Please Enter LastName!");
            UpLastNameLen = UpLastName.length();
            while(UpLastNameLen == 0)
            {
              
              PrevUpdateCancel = millis();
              CardUpdateTimeCurrent = millis();
              CardUpdateTimePrev = millis();
              UpLastName = Serial3.readString();
              UpLastNameLen = UpLastName.length();
              if(UpLastNameLen != 0)
              {
                UpLastNameBool = true;
                Serial3.println(UpLastName);
                Serial3.print(UpFirstName); Serial3.print(" "); Serial3.println(UpLastName);
                Serial3.println("Are you sure?");
                Serial3.println("(y) - is yes, (n) is no");
                while(UpLastNameBool)
                {
                  
                  PrevUpdateCancel = millis();
                  CardUpdateTimeCurrent = millis();
                  CardUpdateTimePrev = millis();
                  input_a = Serial3.read();
                  if((input_a == 'y') || (input_a == 'Y'))
                  {
                    //Send Req Data To Pi MySQLDB
                    MySQLCardDataUpdate = MySQLCardDataUpdateID + MySQLSeparator + UpFirstName + MySQLSeparator + UpLastName + MySQLSeparator;
                    Serial3.print("Data to MYSQL to update Card: "); Serial3.println(MySQLCardDataUpdate);
                    //FirstName = "";
                    UpFirstNameBool = false;
                    //LastName = "";
                    UpLastNameBool = false;
                    Serial.println(MySQLCardDataUpdate);
                    //unsigned long CardUpdateTimeCurrent;
                    //unsigned long CardUpdateTimePrev = 0;
                    //const long CardUpdateTimeoutInterval = 10000;
                    String MySQLUserReturnByte;
                    boolean FindUser = true;
                    long Result = 0;
                    while(FindUser)
                    {
                      
                      PrevUpdateCancel = millis();
                      CardUpdateTimeCurrent = millis();
                      Serial3.print("Current Time: "); Serial3.println(CardUpdateTimeCurrent);
                      Serial3.print("Prev Time: "); Serial3.println(CardUpdateTimePrev);
                      Result = CardUpdateTimeCurrent - CardUpdateTimePrev;
                      Serial3.print("Result: "); Serial3.println(Result);
                      MySQLUserReturnByte = Serial.readString();
                      if(CardUpdateTimeCurrent - CardUpdateTimePrev >= CardUpdateTimeoutInterval)
                      {
                        CardUpdateTimePrev = CardUpdateTimeCurrent;
                        Serial3.println("Failed to get back data from DB.");
                        delay(3000);
                        main_menu = 1;
                        loop();
                      }
                      if(MySQLUserReturnByte == "214")
                      {
                        Serial3.print("Got the User Data: "); Serial3.println(MySQLUserReturnByte);
                        Serial3.println("User Found!");
                        CardUpdateTimePrev = CardUpdateTimeCurrent;
                        FindUser = false;
                        Serial3.println("Please Give the new card!");
                        MySQLCardDataToDBID = "47";
                        MySQLCardDataToDB = MySQLCardDataToDBID + MySQLSeparator + UpFirstName + MySQLSeparator + UpLastName + MySQLSeparator;
                        //Itt tartok, mar csak a mysql updatet kell megcsinalni
                        CardLearningMaster();
                      }else if(MySQLUserReturnByte == "212")
                      {
                        Serial3.print("Got the User Data: "); Serial3.println(MySQLUserReturnByte);
                        Serial3.println("No User Found!");
                        Serial3.println("Maybe You missed some Characters!");
                        //CardUpdateTimePrev = CardUpdateTimeCurrent;
                        FindUser = false;
                        Serial3.println("Would you like to try again? (y) - is yes, (n) is no)");
                        updatetry = true;
                        //CurrUpdateCancel = millis();
                        //UpdateCancelInterval = 30000;
                        
                        
                        while(updatetry)
                        {
                          
                          CurrUpdateCancel = millis();
                          //Serial3.print("Current UpTime: "); Serial3.println(CurrUpdateCancel);
                          Serial3.print("Prev UpTime: "); Serial3.println(PrevUpdateCancel);
                          CurrentUpdateCancelequ = CurrUpdateCancel - PrevUpdateCancel;
                          Serial3.print("Timeout at UpdateCancel: "); Serial3.println(CurrentUpdateCancelequ);
                          if(CurrUpdateCancel - PrevUpdateCancel >= UpdateCancelInterval)
                          {
                            PrevUpdateCancel = CurrUpdateCancel;
                            Serial3.print("Prev UpTime after: "); Serial3.println(PrevUpdateCancel);
                            Serial3.println("Waiting for log/action");
                            readfromoutsideLoRa();
                          }
                          input_a = Serial3.read();
                          if(input_a == 'y' || input_a == 'Y')
                          {
                            Serial3.println("Yes is selected");
                            PrevUpdateCancel = CurrUpdateCancel;
                            CardUpdateToDB();
                          }else if(input_a == 'n' || input_a == 'N')
                          {
                            Serial3.println("No is selected");
                            PrevUpdateCancel = CurrUpdateCancel;
                            Serial3.println("Waiting for log/action");
                            readfromoutsideLoRa();
                          }
                        }
                        
                      }
                    }
                    
                  }else if((input_a == 'n') || (input_a == 'N'))
                  {
                    Serial3.print("Name: "); Serial3.print(UpFirstName); Serial3.print(" "); Serial3.print(UpLastName); Serial3.println(" Deleted.");
                    //Itt is csereltem
                    //UpFirstName = "";
                    UpFirstName.remove(0, UpFirstName.length());
                    UpFirstNameBool = false;
                    //Itt is csereltem
                    //UpLastName = "";
                    UpLastName.remove(0, UpLastName.length());
                    UpLastNameBool = false;
                    CardUpdateToDB();
                  } 
                }
              }
            }
          }

        }
      Serial3.println("Valamiert ide kerulok...");
      }else if(UInput == '2')
      {
        UInput = 0;
        MySQLCardDataUpdateID = "27";
        Serial3.println("Update User's Card by User's ID SELECTED");
        Serial3.println("Please Enter User's ID!");
        boolean UIDBool, UIDBoolSure, ChngUID;
        UIDBool = true;
        UIDBoolSure = true;
        ChngUID = true;
        unsigned int UuserIDLen;
        //UuserID.remove(0, 10000);
        UuserIDLen = UuserID.length();
        while(UIDBool)
        {
          
          UuserID = Serial3.readString();
          //Serial3.print("User ID is: ");Serial3.println(UuserID);
          UuserIDLen = UuserID.length();
          //Serial3.print("User ID length is: ");Serial3.println(UuserIDLen);
          if (UuserIDLen != 0)
          {
            Serial3.print("Got the User ID: "); Serial3.println(UuserID);
            Serial3.println("Are you sure?");
            Serial3.println("(y) - is yes, (n) is no");
            while (UIDBoolSure)
            {
              
              input_a = Serial3.read();
              if((input_a == 'y') || (input_a == 'Y'))
              {
                MySQLCardDataUpdate = MySQLCardDataUpdateID + MySQLSeparator + UuserID + MySQLSeparator;
                Serial3.print("Data to MYSQL to update Card: "); Serial3.println(MySQLCardDataUpdate);
                Serial.println(MySQLCardDataUpdate);
                boolean FindUserID = true;
                String MySQLUserIDReturnByte;
                while(FindUserID)
                {
                  
                  MySQLUserIDReturnByte = Serial.readString();
                  if(MySQLUserIDReturnByte == "528")
                  {
                    Serial3.println("User ID Found!");
                    Serial3.println("Please Give the new card!");
                    MySQLCardDataToDBID = "48";
                    MySQLCardDataToDB = MySQLCardDataToDBID + MySQLSeparator + UuserID + MySQLSeparator;
                    CardLearningMaster();
                    UInput = 0;
                  }else if(MySQLUserIDReturnByte == "529")
                  {
                    Serial3.println("No User ID Found!");
                    Serial3.println("Waiting for log/action");
                    readfromoutsideLoRa();
                  }
                }
              }else if((input_a == 'n') || (input_a == 'N'))
              {
                input_a = 0;
                Serial3.print("User ID: "); Serial3.print(UuserID); Serial3.println(" Deleted.");
                Serial3.println("Would you like to change User ID?");
                Serial3.println("(y) - is yes, (n) is no");
                while(ChngUID)
                {
                  
                  input_a = Serial3.read();
                  if((input_a == 'y') || (input_a == 'Y'))
                  {
                    UInput = 50;
                    //return UInput; // it's character 2 in ASCII
                    CardUpdateToDB();

                  }else if((input_a == 'n') || (input_a == 'N'))
                  {
                    
                    Serial3.println("Waiting for log/action");
                    readfromoutsideLoRa();
                  }
                }
              }
            }
            
          }
        }
      Serial3.println("Ide jottem valamiert.....");
      }
    //}
  } while (CardUpdateBool);
  
}