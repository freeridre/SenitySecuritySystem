//ESP32 LORA SIDE kapu
#include <HardwareSerial.h>
#include "heltec.h"
//#include "arduino.h"
#define BAND    433E6  //915E6
HardwareSerial MEGA(1);
int BackFromDoorStatus;
uint8_t uidLength;
uint8_t byteFromSerial = 0;
uint8_t CARDDATA[12]={255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255};
uint8_t DATA;
uint8_t availableData;
int packetSize;
byte AvailableDataFromSE;
void LORASEND();
void ReceiveDataFromGateArduino();
void SendControlDataToArduino();
boolean ReceiveDataFromInSideLoRa();
#define RESETFROMARDUINO  111
void setup()
{
MEGA.begin(115200,SERIAL_8N1, 36, 12); //RX, TX
Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
Serial.println("Hello It's LoRaOutSide.");
Serial.println("Waiting For InComing Data...");
}
void loop()
{
    if(ReceiveDataFromInSideLoRa())
    {
        SendControlDataToArduino();
        BackFromDoorStatus = 0;
    }
    ReceiveDataFromGateArduino();
    //delay(200);
}
void ReceiveDataFromGateArduino()
{
  availableData = MEGA.available();
  //Serial.print("Available data: ");
  //Serial.println(availableData, HEX);
  AvailableDataFromSE = MEGA.read();
  while (AvailableDataFromSE == RESETFROMARDUINO)
  {
    
    AvailableDataFromSE = 0;
    MEGA.flush();
    delay(2000);
    Serial.println("LoRa OutSide Has Just Reseted! Cause of Arduino OutSide Reset!");
    ESP.restart();
    //loop();
  }
  availableData = MEGA.available();
  //Serial.println("ITT2");
  //while(1);
  while(availableData > 0)
  //while(MEGA.available() > 0)
  {
    for (int i = 2; MEGA.available() > 0; i++)
    {
    byteFromSerial = MEGA.read();
    CARDDATA[i] = byteFromSerial;
    }
    /*
    if (((CARDDATA[2] == CARDDATA[6]) && (CARDDATA[3] == CARDDATA[7]) && (CARDDATA[4] == CARDDATA[8]) && (CARDDATA[5] == CARDDATA[9])) || ((CARDDATA[10] != 255) && (CARDDATA[11] != 255)))
    {
      for (int z = 6; z < 10; z++)
      {
        CARDDATA[z] = 0;
      }
        for (int n = 10; n < 12; n++)
        {
          CARDDATA[n] = 255;
        }
    }else if (CARDDATA[2] == CARDDATA[9])
      {
        CARDDATA[9] = 0;
        for (int n = 10; n < 12; n++)
        {
          CARDDATA[n] = 255;
        }
      }*/
    Serial.println("Card Data From Gate: ");
    for (int x = 0; x < sizeof(CARDDATA); x++)
    {
        Serial.print("0x");
        Serial.print(CARDDATA[x], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");
    Serial.println();
    LORASEND();
  }
  byteFromSerial = 0;
}
void LORASEND()
{
    LoRa.beginPacket();
    Serial.println("Send Card Data From Gate To Inside LoRa.");
    LoRa.write(CARDDATA, sizeof(CARDDATA));
    LoRa.endPacket();
    Serial.println("Data Has Sent To InSide LoRa.");
    Serial.println("Released Card Data Buffer: ");
    for (int i = 2; i < sizeof(CARDDATA); i++)
    {
      CARDDATA[i] = 0;
      Serial.print("0x"); Serial.print(CARDDATA[i]);
    }
    Serial.println();
}
boolean ReceiveDataFromInSideLoRa()
{
  // Stores the return value
  boolean ret = false;
  
  // Receive data from LoRa
  packetSize = LoRa.parsePacket();
  
  if (packetSize)
  {
     // received a packet
    Serial.print("Received Packet From InSide LoRa: ");
    // read packet
    while (LoRa.available())
    {
      BackFromDoorStatus = LoRa.read();
    }
    Serial.println(BackFromDoorStatus, DEC);
    //SendControlDataToArduino();
    ret = true;
  }
  else 
  {
    ret = false;
  }
  return ret;
  
}
void SendControlDataToArduino()
{
MEGA.write(BackFromDoorStatus);
}