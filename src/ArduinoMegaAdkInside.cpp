//ARDUINO INSIDE
//Include libraries
#include "arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "EEPROM.h"
#include "Adafruit_PN532.h"
#include "Nextion.h"
//Define SPI Pins
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
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
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uidLength;
uint8_t Timeout = 0;
//const int eeAddress;
//User Serial Input
int input_a = 0;
//EEPROM
int address = 0;
byte value;
byte value2;
byte value3;
byte valuekartyadb;
int carddb = 0;
//quit
bool Exit = Serial.available();

void quit()
{
  while (1)
  {
    input_a = Serial.read();
      if (input_a == 6)
      {
        Serial.println("||||||||||||||||||||||");
        Serial.println("1 - CardLearning \n");
        Serial.println("2 - CardListing");
        Serial.println("||||||||||||||||||||||\n");
        return;
        loop();
      }
  }

}

void EEPROMREADOUT(){
  valuekartyadb = EEPROM.read(kartyadbeeprom);
  Serial.println("EEPROM READ OUT ALL ONLY REGISTERED CARDS!\n");
  address = 0;
  for (int i = 0; i<valuekartyadb; i++)
  {
    value = EEPROM.read(address);
    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, HEX);
    Serial.println("\n");
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
    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, HEX);
    Serial.println();
    address = address + 1;
      if (address == EEPROM.length()) {
        address = 0;
      }
    delay(5);
    }
    }*/
    Serial.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
    //return;
    quit();
  }
void EEPROMDELETE()
{
  address = 0;
  value = 0;
  for (int i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, value);
    Serial.print(address, DEC);
    Serial.print("\t");
    Serial.println(value);
    address = address + 1;
  }
    Serial.println("A teljes EEPROM torolve lett!");
    kartyadb = 1;
    EEPROM.write(kartyadbeeprom, kartyadb);
    //int eepromcim = EEPROM.read(eepromcimeeprom);
  quit();
}
void EEPROMREADOUTALL(){
  Serial.println("EEPROM READ OUT ALL ONLY REGISTERED CARDS!\n");
  address = 0;
  for (int i = 0; i<EEPROM.length(); i++)
  {
    value = EEPROM.read(address);
    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, HEX);
    Serial.println("\n");
    address = address + 1;
    if (address == EEPROM.length())
    {
      address = 0;
    }
  }
    Serial.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
    quit();
  }
//Start NFC Init
void NFCINITIALIZE ()
{
 Serial.println("NFC initialization in progress...");
 //Start NFC Comm.
 nfc.begin();
 //Start SAM
 nfc.SAMConfig();
 //Get NFC module GetFirmwareVersion
 uint32_t versiondata = nfc.getFirmwareVersion();
 if (! versiondata)
 {
   Serial.print("Didn't find PN53x board");
   //halt
   while (1);
 }
 //Print out chipversion
 Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
 Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
 Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
 Serial.print("\n");
 
}
//Kartyatanitas EEPROM
void CardLearning()
{
 do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    Serial.println("-------------------------------------------------------------------------------");
    Serial.print("Kerem a""(z) "); Serial.print(kartyadb); Serial.print(". taget/kartyat. \n");
    Serial.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    Serial.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
    //Scan RFID UID
    while (1)
    {
      quit();
      return;
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, Timeout);
      
    }
    Serial.println("A taget/kartyat beolvastuk! \n");
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
        /*Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
        Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");*/
        ID++, address++;
        //Serial.println("Find the"); Serial.print(ID); Serial.print(" byte.");
        value = EEPROM.read(address);
        if(value == uid[ID])
        {
          /*Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
          Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");*/
          ID++, address++;
          value = EEPROM.read(address);
          if(value == uid[ID])
          {
            /*Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
            Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");*/
            ID++, address++;
            value = EEPROM.read(address);
            if(value == uid[ID])
            {
              /*Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
              Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");*/
              ID++, address++;
              value = EEPROM.read(address);
              Serial.println("Ez a kartya/tag mar fel lett veve! Kerlek valasz egy masikat!");
              delay(5000);
              CardLearning();
            }
            address++;
            
          }
          address++;
          
        }
        address++;
        
      }
      address++;
      
    }




    Serial.print ("A(z) "); Serial.print(kartyadb); Serial.print(". TAG/KARTYA UID:"); nfc.PrintHex(uid, uidLength);
    if (uidLength == 4)
    {
      Serial.println("Ez egy Mifare Classic kartya 4 byte UID.");
      //EEPROM WRITE SCANNED RFID UID
      eeAddress = EEPROM.read(eeAddresseeprom);
      EEPROM.put(eeAddress, uid);
      eepromcim = EEPROM.read(eepromcimeeprom);
      Serial.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial.print(eeAddress); Serial.print(". bytetol kezdve! \n");
      eeAddress=eeAddress+5;
      EEPROM.put(eeAddresseeprom,eeAddress);
      eepromcim=eepromcim+5;
      EEPROM.write(eepromcimeeprom,eepromcim);
      //delay(2000);
      kartyadb++;
      EEPROM.put(kartyadbeeprom, kartyadb);
      delay(2000);
      Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
      Serial.println("A kovetkezo TAG/KARTYA felvetelehez kerlek erintsd meg a kijelzőt!");
      Serial.println("A fomenube valo visszajutashoz kerlek nyomd meg a vissza gombot!");
      Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
      Serial.println("------------------------------------------------------------------------------- \n");
      //eeAddress = EEPROM.read(eeAddresseeprom);
      //ide kell az erintes figyelese
      while ((digitalRead(13) == LOW) && (digitalRead(8) == LOW));
    }
    else if (uidLength == 7)
    {
      Serial.println("Ez egy Mifare Ultralight kartya 7 byte UID jelenleg nem támogatott!");
      CardLearning();
      //EEPROM WRITE SCANNED RFID UID
      EEPROM.put(eeAddress, uid);
      eeAddress=eeAddress+8;
      EEPROM.put(eeAddresseeprom,eeAddress);
      Serial.print("A uid-t mentettuk az EEPROM-ba a""(z) "); Serial.print(eepromcim); Serial.print(". bittol kezdve! ");
      eepromcim = eepromcim+8;
      EEPROM.put(eepromcimeeprom,eepromcim);
      //delay(2000);      
      kartyadb++; 
      EEPROM.put(kartyadbeeprom, kartyadb);
      Serial.println();
      Serial.println();
      Serial.println("A kovetkezo TAG/KARTYA felvetelehez kerlek nyomd meg az 1-es gombot!");
      Serial.println("A fomenube valo visszajutashoz kerlek nyomd meg a 6-os gombot!");
      while ((digitalRead(13) == LOW) && (digitalRead(8) == LOW)); 
    }
    else if (uidLength > 7)
    {
      Serial.println("A KARTYA/TAG nem kompatibilis!");
      CardLearning();
    }
  }
  while (digitalRead(13) == HIGH);
}

void setup() {
  // put your setup code here, to run once:
  //Serial0 start
  Serial.begin(115200);
  //Welcome
  Serial.println("Welcome!");
  //NFC Module Init
  NFCINITIALIZE ();
  Serial.println("1 - CardLearning \n");
  Serial.println("2 - CardListing \n");
  Serial.println("3 - EEPROM Delet \n");
  Serial.println("4 - EEPROM Read Out All \n");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Card Learning
  while (Serial.available() > 0)
  {
    input_a = Serial.read();
    if (input_a == 1)
    {
      CardLearning();
    }
    else if(input_a == 2)
    {
      EEPROMREADOUT();
    }
    else if(input_a == 3)
    {
      EEPROMREADOUT();
    }
    else if(input_a == 4)
    {
      EEPROMREADOUTALL();
    }
    
    
  }
  //CardLearning();
}