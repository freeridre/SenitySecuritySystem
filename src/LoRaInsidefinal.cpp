//ESP32 LORA SIDE bent
#include <HardwareSerial.h>
#include "heltec.h"

#define BAND 868E6   //433E6  //you can set band here directly,e.g. 868E6,915E6
HardwareSerial MEGA(1);
uint8_t CardDataFromKapu[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t byteFromKapu;
uint8_t CARDDATATOINSIDEMEGA;
uint8_t BackFromSenityStatus;
int packetSize = LoRa.parsePacket();
boolean ReceiveDataFromLoRa();
void SendCardDataToArduino();
void ReceiveDataFromArduino();
void GetControlDatacheckFromOutSideLoRa();
void setup() {
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

void ReceiveDataFromArduino() {

  //uint8_t availableData = MEGA.available();
  /*Serial.print("Available data: ");
  Serial.println(availableData, HEX);*/
            
  
  while(MEGA.available())
  {    
    BackFromSenityStatus = MEGA.read();
    Serial.print("Received Data From InSide Arduino: ");
    Serial.println(BackFromSenityStatus, DEC);
    if (BackFromSenityStatus == 112)
    {
      Serial.println("LoRa Inside Has Just Reseted! Cause of Arduino Inside Reset!");
      BackFromSenityStatus = 0;
      ESP.restart();
    }
    //delay(250);
    LoRa.beginPacket();
    //Serial.print("Send Recieved Data ");
    //Serial.print(BackFromSenityStatus);
    //Serial.println("From Arduino to Gate LoRa.");
    //MEGA.println("Send Data to Gate LoRa");
    Serial.print("Send Received Data "); Serial.print(BackFromSenityStatus); Serial.println(" From Arduino to Gate LoRa.");
    //LoRa.write(BackFromSenityStatus);
    LoRa.write(BackFromSenityStatus);
    LoRa.endPacket();
    Serial.println("End Data Sending to Gate LoRa");
    //MEGA.println("End Data Sending to Gate LoRa");
  }
  BackFromSenityStatus = 0;
}



void SendCardDataToArduino()
{  
  for(int i = 0; i < sizeof(CardDataFromKapu); i += 1)
  {
      MEGA.write(CardDataFromKapu[i]);
  }
}
void GetControlDatacheckFromOutSideLoRa()
{
  int ControlPacket = LoRa.parsePacket();

}