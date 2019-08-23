//ARDUINO INSIDE
//COMS
//Arduino Outside COM7
//Arduino Inside COM5
//LORA Outside COM
//LORA Inside COM21
//Include libraries
#include "arduino.h"
#include "Wire.h"
#include "SPI.h"
#include "EEPROM.h"
#include "Adafruit_PN532.h"
#include "Nextion.h"
#include "SparkFun_Qwiic_Rfid.h"
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
unsigned int i = 0;
unsigned int TagLength = 0;
int ten = 10;
int eleven = 11;
uint8_t IntegerDataStringArray1[4] = {};
//LoRa
uint8_t byteFromESP32 [12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Status;
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
int RFIDControlByte = 0;
bool Exit = Serial.available();
int back = 1;
//menu Loop()
int main_menu = 1;
void readfromoutsideLoRa();
void CardLearningV2();
void AccessControl();
void CardReadOut()
{
  Serial.println("EEPROM READ OUT ALL ONLY REGISTERED CARDS!\n");
  int a_valuekartyadb = EEPROM.read(kartyadbeeprom) -1;
  valuekartyadb = a_valuekartyadb*5; 
  address = 0;
  for (int i = 0; i<valuekartyadb; i++)
  {
    value = EEPROM.read(address);
    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, HEX);
    Serial.println("\t");
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
   while (6 != 7)
    {
      input_a = Serial.read();
        if (input_a == 6)
        {
          main_menu = 1;
          loop();
        }
    }
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
    main_menu = 1;
    loop();
}
void EEPROMREADOUTALL()
{
  Serial.println("EEPROM READ OUT ALL!\n");
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
  //quit
  Serial.println("A visszalepeshez nyomd meg a 6-os gombot!\n");
   while (6 != 7)
    {
      input_a = Serial.read();
        if (input_a == 6)
        {
          main_menu = 1;
          loop();
        }
    }
}
//Start NFC Init
void NFCINITIALIZE ()
{
 Serial.println("NFC initialization in progress...");
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
/*void CardLearning()
{
 do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    Serial.println("-------------------------------------------------------------------------------");
    Serial.print("Kerem a""(z) "); Serial.print(kartyadb); Serial.print(". taget/kartyat. \n");
    Serial.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    Serial.println("A tovabb lepeshez nyomd meg barmelyik gombot (1,2,3,4,5), a visszalepeshez nyomd meg a 6-os gombot!\n");
    //quit
    while (input_a != 7)
    {
      input_a = Serial.read();
        if (input_a == 6)
        {
          main_menu == 1;
          Serial.println("||||||||||||||||||||||");
          Serial.println("1 - Card Learning \n");
          Serial.println("2 - Card Listing \n");
          Serial.println("3 - EEPROM Delete \n");
          Serial.println("4 - EEPROM Read Out All \n");
          Serial.println("5 - Access Control");
          Serial.println("||||||||||||||||||||||\n");
          loop();
          return;
        }
        else if ((input_a == 1) || (input_a == 2) || (input_a == 3) ||(input_a == 4) || (input_a == 5))
        {
        break;
        }
    }
    Serial.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    //Scan RFID UID
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, Timeout);
    
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
        Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
        Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
        ID++, address++;
        //Serial.println("Find the"); Serial.print(ID); Serial.print(" byte.");
        value = EEPROM.read(address);
        if(value == uid[ID])
        {
          Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
          Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
          ID++, address++;
          value = EEPROM.read(address);
          if(value == uid[ID])
          {
            Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
            Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
            ID++, address++;
            value = EEPROM.read(address);
            if(value == uid[ID])
            {
              Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
              Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
              ID++, address++;
              value = EEPROM.read(address);
              Serial.println("Ez a kartya/tag mar fel lett veve! Kerlek valasz egy masikat!");
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
void RelayBuzzer()
{
analogWrite(4, HIGH);
Serial.println("||||||||||||||||||||||");
Serial.println("1 - Card Learning \n");
Serial.println("2 - Card Listing \n");
Serial.println("3 - EEPROM Delete \n");
Serial.println("4 - EEPROM Read Out All \n");
Serial.println("5 - Access Control");
Serial.println("||||||||||||||||||||||\n");
loop();
return;
}
*/
void AccessControl()
{
 do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    
    Serial.println("-------------------------------------------------------------------------------");
    Serial.print("Kerem a "); Serial.print("taget/kartyat. \n");
    Serial.println("Helyezd az ISO14443A kartyat az olvasohoz ...");
    //Scan RFID UID
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, Timeout);
    Serial.println("A taget/kartyat beolvastuk! \n");
    
    //Leellenorizzuk, hogy mar fel van-e veve ez a UID
    byte value;
    //Az első kettő FF, FF
    int ID = 3;
    carddb = kartyadb * uidLength;
    address = 0;
    for (int i = 0; i < carddb; i++)
    {
      value = EEPROM.read(address);
      if(value == byteFromESP32[ID])
      {
        Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
        Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
        ID++, address++;
        //Serial.println("Find the"); Serial.print(ID); Serial.print(" byte.");
        value = EEPROM.read(address);
        if(value == byteFromESP32[ID])
        {
          Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
          Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
          ID++, address++;
          value = EEPROM.read(address);
          if(value == byteFromESP32[ID])
          {
            Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
            Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
            ID++, address++;
            value = EEPROM.read(address);
            if(value == byteFromESP32[ID])
            {
              Serial.println("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| \n");
              Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
              ID++, address++;
              value = EEPROM.read(address);
              Serial.println("Passed! \n");
              //Itt van a hiba!!
              //RelayBuzzer();
              //vissza a LoRa-ra
              Status = 12;
              digitalWrite(35, HIGH);
              //delay(2000);
              Serial1.write(Status);
              Serial.print("Status "); Serial.print(Status); Serial.print(" has written back to LoRa! \n");
              digitalWrite(35, LOW);
              Serial.println("Waiting for incoming data...");
              
              return;
              
              digitalWrite(36, HIGH);
              delay(100);
              digitalWrite(36, LOW);
              //delay(2000);
              Serial.println("Waiting for incoming data...");
              //readfromoutsideLoRa();
              return;
              
              //Serial.println("STOP!");
            }
            address++;
          }
          address++;
        }
        address++;
      }
      address++;
    }
    Serial.println("Nem jogosult! \n");
    //delay(2000);
    Status = 11;
    digitalWrite(35, HIGH);
    Serial1.write(Status);
    Serial.print("Status "); Serial.print(Status); Serial.print(" has written back to LoRa! \n");
    digitalWrite(35, LOW);
    Serial.println("Waiting for incoming data...");
    return;
  }while (10 != 11);
}
void readfromoutsideLoRa()
{
    input_a = Serial.read();
    if (input_a == 6)
    {
      main_menu = 1;
      loop();
      return;
    }
    char i = 0;
    while(Serial1.available() > 0)
    {
      //Orange Led shows serial communication progress
      digitalWrite(35, HIGH);
      byteFromESP32[i] = Serial1.read();     
      Serial.print(byteFromESP32[i], HEX);
      i += 1;
    }
    digitalWrite(35, LOW);
    
    if(i != 0)
    {
        Serial.print('\n');
        Serial.print("Bytes received: ");
        Serial.print(i, DEC);
        Serial.print('\n');
        Serial.print('\n');
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
      Serial.print("UID Lenght: "); Serial.println(uidLength);
      //uidLength = 0;
      AccessControl();
      //return;
    }
  //}while(1 > 0);
  readfromoutsideLoRa();
  return;
}
void setup() {
  // put your setup code here, to run once:
  //Serial0 start
  Serial.begin(115200);
  //Welcome
  Serial.println("Welcome!");
  //Begin I2C
  Wire.begin();
  //Test I2C Communication
  if(myRfid.begin())
    Serial.println("I2C RFiD Ready!"); 
  else
    Serial.println("Could not communicate with Qwiic RFID!");

  //Serial1 start
  Serial1.begin(115200);
  //magnes
  pinMode(36, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(4, OUTPUT);
  //NFC Module Init
  NFCINITIALIZE ();
  /*
  Serial.println("||||||||||||||||||||||");
  Serial.println("1 - Card Learning \n");
  Serial.println("2 - Card Listing \n");
  Serial.println("3 - EEPROM Delete \n");
  Serial.println("4 - EEPROM Read Out All \n");
  Serial.println("5 - Access Control");
  Serial.println("||||||||||||||||||||||\n");
  */
}
void loop()
{
  if (main_menu == 1)
  {
  Serial.println("||||||||||||||||||||||");
  Serial.println("1 - Card Learning \n");
  Serial.println("2 - Card Listing \n");
  Serial.println("3 - EEPROM Delete \n");
  Serial.println("4 - EEPROM Read Out All \n");
  Serial.println("5 - Access Control");
  Serial.println("||||||||||||||||||||||\n");
  main_menu = 0;
  input_a = 0;
  }
    input_a = Serial.read();
    if (input_a == 1)
    {
      CardLearningV2();
    }
    else if(input_a == 2)
    {
      CardReadOut();
    }
    else if(input_a == 3)
    {
      EEPROMDELETE();
    }
    else if(input_a == 4)
    {
      EEPROMREADOUTALL();
    }
    else if(input_a == 5)
    {
      Serial.println("Waiting for incoming data...");
      readfromoutsideLoRa();

    }
}
void CardLearningV2()
{
do
 {
    nfc.SAMConfig();
    //kártyadb bekerese EEPROMbol, illetve kartya kerese
    kartyadb = EEPROM.read(kartyadbeeprom);
    Serial.println("-------------------------------------------------------------------------------");
    Serial.print("Kerem a""(z) "); Serial.print(kartyadb); Serial.print(". taget/kartyat. \n");
    Serial.println("A visszalepeshez ird be a 6-ost!");
    //Scan RFID UID
    while(success == 0 && tag.toInt() == 0)
    {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    tag = myRfid.getTag();
    input_a = Serial.read();
    //Serial.println("Waiting for Card...");
      if (input_a == 6)
      {
        main_menu = 1;
        input_a = 0;
        loop();
        return;
      }
    }
    if(tag.toInt() != 0)
    {
      unsigned int TagLength = tag.length();
      //Serial.print("Current TagLength: "); Serial.println(TagLength);
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
      Serial.println();
      //Release variables
      b = 1;c = 0;i = 0;RFIDID = 14;
      for (int p = 0; p < 4; p++)
      {
        //Serial.print(uid[p]);
      uidLength++;
      }
      Serial.println();
    }
    if (success > 0)
    {
      RFIDControlByte = 1;
      success = 0;
    }
    if (tag.toInt() != 0)
    {
      RFIDControlByte = 2;
      //tag.toInt() = 0;
    }
    if(RFIDControlByte == 1 || RFIDControlByte == 2)
    {  
      Serial.println("A taget/kartyat beolvastuk! \n");
       Serial.print ("A(z) "); Serial.print(kartyadb); Serial.print(". TAG/KARTYA UID:"); 
      if (RFIDControlByte == 1)
      {
      nfc.PrintHex(uid, uidLength);
      }
      else if (RFIDControlByte == 2)
      {
      for (int p = 0; p < 4; p++)
        {
        Serial.print("0x");Serial.print(uid[p], HEX);Serial.print(" ");
        }
      Serial.println();
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
          //Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
          ID++, address++;
          //Serial.println("Find the"); Serial.print(ID); Serial.print(" byte.");
          value = EEPROM.read(address);
          if(value == uid[ID])
          {
            //Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
            ID++, address++;
            value = EEPROM.read(address);
            if(value == uid[ID])
            {
              //Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
              ID++, address++;
              value = EEPROM.read(address);
              if(value == uid[ID])
              {
                //Serial.print("A UID "); Serial.print(ID); Serial.print(". "); Serial.print("byte-ja benne van az EEPROM "); Serial.print(address);Serial.print(". "); Serial.print("cimeben! \n");
                ID++, address++;
                value = EEPROM.read(address);
                Serial.println("Ez a kartya/tag mar fel lett veve! Kerlek valasz egy masikat!");
                Serial.println("A visszalepeshez ird be a 6-ost!");
                delay(5000);
                RFIDControlByte = 0;
                for (o = 0; o < uidLength; o++)
                {
                  uid[o] = 0;
                }
                uidLength = 0;
                for (unsigned int u; u < 16; u++)
                {
                  IntegerDataStringArray[u] = 0;
                }
                TagLength = 0;
                ID = 0; address = 0;
                input_a = 0;
                tag = "";
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
        Serial.println("Ez egy Mifare Classic kartya 4 byte UID vagy RFID TAG.");
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
        Serial.println("------------------------------------------------------------------------------- \n");
      }
      else if (uidLength == 7)
      {
        Serial.println("Ez egy Mifare Ultralight kartya 7 byte UID jelenleg nem támogatott!");
        CardLearningV2();
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
      }
      else if (uidLength > 7)
      {
        Serial.println("A KARTYA/TAG nem kompatibilis!");
        CardLearningV2();
      }
      RFIDControlByte = 0;
    tag = "";
    //Serial.println(TagLength);
    //Serial.println(uidLength);
    
    for (int o = 0; o < uidLength; o++)
      {
        uid[o] = 0;
      }
      for (int p2 = 0; p2 < 4; p2++)
      {
        uid[p2] = 0;
        uidLength = 0;
      }
      Serial.println();
      for (unsigned int u; u < TagLength; u++)
      {
        IntegerDataStringArray[u] = 0;
      }
      TagLength = 0;
    }
    
  }while ( 11 != 10);
}