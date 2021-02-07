//ESP32 LORA SIDE bent
#include <HardwareSerial.h>
#include "heltec.h"
#include <esp_task_wdt.h>
#include <vector>
#include <iostream>
#define BAND 868E6   //433E6  //you can set band here directly,e.g. 868E6,915E6
HardwareSerial MEGA(2);
uint8_t CardDataFromKapu[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t byteFromKapu;
uint8_t CARDDATATOINSIDEMEGA;
uint8_t BackFromSenityStatus[6] = {};
uint8_t UIDDownload[5] = {0, 0, 0, 0, 0};
int CountBackFromSenityStatus = 0;
int packetSize = LoRa.parsePacket();
int NOCP = 0;
int NOC = 0;
boolean ReceiveDataFromLoRa();
void SendCardDataToArduino();
void ReceiveDataFromArduino();
void GetControlDatacheckFromOutSideLoRa();
//Display outputs
void dispinit();
void disploggot();
void displogtodb();
void displogwait();
void dispgotdatafromdb();
void dispwaitprog();
unsigned long previousMillis = 0;
unsigned long previousMillisdisp = 0;
const long interval = 20000;
const long intervaldispstatus = 500;
const int intervaldisplogtodb = 2000;
unsigned long currentMillis, currentmillisdisp, currentmillisdisplogtodb;
byte dispstatus = 0;
boolean RCDDbool = false;
boolean dispwaitprogbool = true;
boolean displogtodbbool = false;
boolean dispgotdatafromdbbool = false;
boolean dispdatafromlorabool = false;
void dispRetryCardDataDownload();
void setup()
{
  ///****
  esp_task_wdt_init(8, true);

    //WIFI Kit series V1 not support Vext control
  //Serial.begin(115200);
  //MEGA.begin(115200,SERIAL_8N1, 36, 12); //RX, TX
  MEGA.begin(115200,SERIAL_8N1, 13, 12); //RX, TX
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  //LoRa.setTxPowerMax(20);
  Serial.println();
  Serial.println("Hello It's LoRaInSide.");
  Serial.println("Waiting For InComing Data...");
  dispinit();
  delay(5000);
  displogwait();
}

void loop()
{
  currentMillis = millis();
  currentmillisdisp = millis();    
  //MEGA.write("9");
  //Serial.println("Hello2!");
  ///****
  /*if (currentMillis - previousMillis >= interval)
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      displogwait();
    }*/
  if(dispwaitprogbool)
  {
    dispwaitprog();
  }
  esp_task_wdt_reset();

  if(ReceiveDataFromLoRa()) {
    SendCardDataToArduino();
    dispwaitprogbool = true;
  }

  ReceiveDataFromArduino();
  //delay(200);
}

boolean ReceiveDataFromLoRa() {

  // Stores the return value
  boolean ret = false;
  
  // Receive data from LoRa
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    dispwaitprogbool = false;
    dispdatafromlorabool = true;
    displogtodbbool = false;
    dispgotdatafromdbbool = false;
    RCDDbool = false;
     // received a packet
    Serial.print("Received Packet From Gate LoRa: ");
    // read packet
    int i = 0;
    while (LoRa.available() && i < sizeof(CardDataFromKapu)) {
      //Serial.print((char)LoRa.read());
      
      CardDataFromKapu[i] = LoRa.read();
      //Serial.print(byteFromKapu);
      //Serial.println(byteFromKapu.charAt(4));
      i++;
    }
    /*if (CardDataFromKapu[0] == 1 && CardDataFromKapu[1] == 1 && CardDataFromKapu[2] == 1)
    {
      Serial.println(CardDataFromKapu[0]);
      Serial.println(CardDataFromKapu[1]);
      Serial.println(CardDataFromKapu[2]);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus);
      LoRa.endPacket();
      ret = false;
      return ret;
    }else if(CardDataFromKapu[0] == 2 && CardDataFromKapu[1] == 2 && CardDataFromKapu[2] == 2)
    {
      BackFromSenityStatus = 0;
    }*/
    // print RSSI of packet
    //Read DATA From LoRa
    for (int i = 0; i < sizeof(CardDataFromKapu); i++)
    {
      Serial.print(CardDataFromKapu[i], HEX);
    }
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
   ret = true;
  }
  return ret;
}

void ReceiveDataFromArduino() 
{
  
  
  CountBackFromSenityStatus = 0;
  while(MEGA.available() > 0)
  {
    BackFromSenityStatus[CountBackFromSenityStatus] = MEGA.read();
    //MEGA.readBytes(BackFromSenityStatus, 6);
    if(BackFromSenityStatus[0] == 0x2A && BackFromSenityStatus[1] == 0x2A && BackFromSenityStatus[2] == 0x2D && BackFromSenityStatus[4] == 0x2A)
    {
      RCDDbool = true;
      Serial.println("Number of cards data has sent.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      Serial.println(BackFromSenityStatus[3], HEX);
      Serial.println(BackFromSenityStatus[4], HEX);
      Serial.println(BackFromSenityStatus[5], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
      
    }else if(BackFromSenityStatus[0] == 63 && BackFromSenityStatus[1] == 63 && BackFromSenityStatus[2] == 64)
    {
      RCDDbool = true;
      Serial.println("It's the end of Card data from DB.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 113 && BackFromSenityStatus[1] == 113 && BackFromSenityStatus[2] == 239)
    {
      RCDDbool = false;
      Serial.println("Retry Card Data Donwload process Finished.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 0x71 && BackFromSenityStatus[1] == 0x71 && BackFromSenityStatus[2] == 0xF0)
    {
      RCDDbool = true;
      Serial.println("It's Retry Card Data Download Connection.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 0x71 && BackFromSenityStatus[1] == 0x71 && BackFromSenityStatus[2] == 0xF1)
    {
      RCDDbool = false;
      Serial.println("The Database is empty");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }
    CountBackFromSenityStatus++;
  }
  if(CountBackFromSenityStatus != 0)
  {
    int ia = 0;
    Serial.print("Data From Senity is: ");
    for(ia = 0; ia < sizeof(BackFromSenityStatus); ia++)
    {
      if (BackFromSenityStatus[ia] != 0)
      {
        Serial.print(BackFromSenityStatus[ia], HEX); Serial.print(" ");
      }
    }
    Serial.println();
    Serial.print("Data length arrived: "); Serial.println(ia);
  }
  if(BackFromSenityStatus[0] == 0xC)
  {
    Serial.println("Passed");
    LoRa.beginPacket();
    LoRa.write(BackFromSenityStatus[0]);
    LoRa.endPacket();
    Serial.println("Data has sent to outside.");
  }else if(BackFromSenityStatus[0] == 0xB)
  {
    Serial.println("Rejected");
    LoRa.beginPacket();
    LoRa.write(BackFromSenityStatus[0]);
    LoRa.endPacket();
    Serial.println("Data has sent to outside.");
  }else if(BackFromSenityStatus[0] == 255 && BackFromSenityStatus[1] == 254)
  {
    Serial.println("It's a card pkg from DB.");
    NOC++;
    NOCP = NOC;
    for (int i = 0; i < sizeof(BackFromSenityStatus); i++)
    {
      Serial.print(BackFromSenityStatus[i], HEX);
    }
    Serial.println();
    LoRa.beginPacket();
    LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
    LoRa.endPacket();
    for (int i = 0; i < sizeof(BackFromSenityStatus); i++)
    {
      BackFromSenityStatus[i] = 0;
    }
  }
  //Release
  for (int i = 0; i < sizeof(BackFromSenityStatus); i++)
  {
    BackFromSenityStatus[i] = 0;
  }

}



void SendCardDataToArduino()
{ 
  displogtodbbool = true;
  //Serial.println("Kuldon az adatot");
  for(int i = 0; i < sizeof(CardDataFromKapu); i += 1)
  {
      MEGA.write(CardDataFromKapu[i]);
      //Serial.println(CardDataFromKapu[i], HEX);
  }
}
void GetControlDatacheckFromOutSideLoRa()
{
  int ControlPacket = LoRa.parsePacket();

}
void dispinit()
{
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 0, "OLED initial done!");
  Heltec.display->display();
}
void disploggot()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Got Log From Gate!");
  Heltec.display->display();
}
void displogtodb()
{
  if(displogtodbbool && (millis() >= currentmillisdisplogtodb + intervaldisplogtodb))
  {
    currentmillisdisplogtodb += intervaldisplogtodb;
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Log Sent to DB!");
	  Heltec.display->display();
  }else if(displogtodbbool && (millis() <= currentmillisdisplogtodb + intervaldisplogtodb))
  {
    displogtodbbool = false;
  }
}
void displogwait()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Waiting for incoming data!");
  Heltec.display->display();  
}
void dispRetryCardDataDownload()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Retry Card Data Download!");
	Heltec.display->display();
}
void dispgotdatafromdb()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Got data from DB!");
	Heltec.display->display();
}
void dispwaitprog()
{
  if ((currentmillisdisp - previousMillisdisp >= intervaldispstatus) && dispwaitprogbool)
  {
    previousMillisdisp = currentmillisdisp;
    if(dispstatus == 0)
    {
      Heltec.display->clear();
      //Serial.println("0");
      Heltec.display->drawString(0, 12, "-");
      dispstatus = 1;
    }else if(dispstatus == 1)
    {
      Heltec.display->clear();
      //Serial.println("1");
      Heltec.display->drawString(0, 12, "- -");
      dispstatus = 2;
    }else if(dispstatus == 2)
    {
      Heltec.display->clear();
      //Serial.println("2");
      Heltec.display->drawString(0, 12, "- - - ");
      dispstatus = 3;
    }else if(dispstatus == 3)
    {
      Heltec.display->clear();
      //Serial.println("3");
      Heltec.display->drawString(0, 12, "- - - - ");
      dispstatus = 4;
    }else if(dispstatus == 4)
    {
      Heltec.display->clear();
      //Serial.println("4");
      Heltec.display->drawString(0, 12, "- - - - - ");
      dispstatus = 5;
    }else if(dispstatus == 5)
    {
      Heltec.display->clear();
      //Serial.println("5");
      Heltec.display->drawString(0, 12, "- - - - - - ");
      dispstatus = 6;
    }else if(dispstatus == 6)
    {
      Heltec.display->clear();
      //Serial.println("6");
      Heltec.display->drawString(0, 12, "- - - - - - - ");
      dispstatus = 7;
    }else if(dispstatus == 7)
    {
      Heltec.display->clear();
      //Serial.println("7");
      Heltec.display->drawString(0, 12, "- - - - - - - - ");
      dispstatus = 8;
    }else if(dispstatus == 8)
    {
      Heltec.display->clear();
      //Serial.println("8");
      Heltec.display->drawString(0, 12, "- - - - - - - - - ");
      dispstatus = 9;
    }else if(dispstatus == 9)
    {
      Heltec.display->clear();
      //Serial.println("9");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - ");
      dispstatus = 10;
    }else if(dispstatus == 10)
    {
      Heltec.display->clear();
      //Serial.println("10");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - -");
      dispstatus = 11;
    }else if(dispstatus == 11)
    {
      Heltec.display->clear();
      //Serial.println("11");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - ");
      dispstatus = 12;
    }else if(dispstatus == 12)
    {
      Heltec.display->clear();
      //Serial.println("12");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - ");
      dispstatus = 13;
    }else if(dispstatus == 13)
    {
      Heltec.display->clear();
      //Serial.println("13");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - ");
      dispstatus = 14;
    }else if(dispstatus == 14)
    {
      Heltec.display->clear();
      //Serial.println("14");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - ");
      dispstatus = 15;
    }else if(dispstatus == 15)
    {
      Heltec.display->clear();
      //Serial.println("15");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - - ");
      dispstatus = 16;
    }else if(dispstatus == 16)
    {
      Heltec.display->clear();
      //Serial.println("16");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - - - ");
      dispstatus = 17;
    }else if(dispstatus == 17)
    {
      Heltec.display->clear();
      //Serial.println("17");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - - - - ");
      dispstatus = 18;
    }else if(dispstatus == 18)
    {
      Heltec.display->clear();
      //Serial.println("18");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - - - - - ");
      dispstatus = 19;
    }else if(dispstatus == 19)
    {
      Heltec.display->clear();
      //Serial.println("19");
      Heltec.display->drawString(0, 12, "- - - - - - - - - - - - - - - - - - - -");
      dispstatus = 0;
    }
    Heltec.display->drawString(0, 0, "Waiting for incoming data!");
    Heltec.display->drawString(0, 48, "Senity Security Systems");
    Heltec.display->display();
  }
}
