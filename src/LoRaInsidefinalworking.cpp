//ESP32 LORA SIDE bent
#include <HardwareSerial.h>
#include "heltec.h"
#include <esp_task_wdt.h>
#include <vector>
#include <iostream>
#define BAND 868E6   //433E6  //you can set band here directly,e.g. 868E6,915E6
HardwareSerial MEGA(1);
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
void setup() {
  ///****
  esp_task_wdt_init(2, true);

    //WIFI Kit series V1 not support Vext control
  //Serial.begin(115200);
  MEGA.begin(115200,SERIAL_8N1, 36, 12); //RX, TX
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  LoRa.setTxPowerMax(20);
  Serial.println();
  Serial.println("Hello It's LoRaInSide.");
  Serial.println("Waiting For InComing Data...");
}

void loop() {    
  //MEGA.write("9");
  //Serial.println("Hello2!");
  ///****
  esp_task_wdt_reset();

  if(ReceiveDataFromLoRa()) {
    SendCardDataToArduino();
    
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
      Serial.println("It's the end of Card data from DB.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 113 && BackFromSenityStatus[1] == 113 && BackFromSenityStatus[2] == 239)
    {
      Serial.println("Retry Card Data Donwload process Finished.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 0x71 && BackFromSenityStatus[1] == 0x71 && BackFromSenityStatus[2] == 0xF0)
    {
      Serial.println("It's Retry Card Data Donwload Connection.");
      Serial.println(BackFromSenityStatus[0], HEX);
      Serial.println(BackFromSenityStatus[1], HEX);
      Serial.println(BackFromSenityStatus[2], HEX);
      LoRa.beginPacket();
      LoRa.write(BackFromSenityStatus, sizeof(BackFromSenityStatus));
      LoRa.endPacket();
    }else if(BackFromSenityStatus[0] == 0x71 && BackFromSenityStatus[1] == 0x71 && BackFromSenityStatus[2] == 0xF1)
    {
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