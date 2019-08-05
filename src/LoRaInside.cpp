//ESP32 LORA SIDE bent
#include <HardwareSerial.h>
#include "heltec.h"

#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6
HardwareSerial MEGA(1);
uint8_t CardDataFromKapu[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t byteFromKapu;
uint8_t CARDDATATOINSIDEMEGA;
int BackFromSenityStatus;
int packetSize = LoRa.parsePacket();
boolean ReceiveDataFromLoRa();
void SendCardDataToArduino();
void ReceiveDataFromArduino();
void setup() {
    //WIFI Kit series V1 not support Vext control
  //Serial.begin(115200);
  MEGA.begin(115200,SERIAL_8N1, 36, 12); //RX, TX
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  
  Serial.println();
  Serial.println("hello waiting for incoming data...");
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
    Serial.print("Received packet '");
    // read packet
    int i = 0;
    while (LoRa.available() && i < sizeof(CardDataFromKapu)) {
      //Serial.print((char)LoRa.read());
      
      CardDataFromKapu[i] = LoRa.read();
      //Serial.print(byteFromKapu);
      //Serial.println(byteFromKapu.charAt(4));
      i++;
    }
    // print RSSI of packet
    //Read DATA From LoRa
    for (int i = 0; i < sizeof(CardDataFromKapu); i++){
      Serial.print(CardDataFromKapu[i], HEX);
    }
    Serial.print("' with RSSI ");
   Serial.println(LoRa.packetRssi());
   ret = true;
  }
  return ret;
}

void ReceiveDataFromArduino() {

  uint8_t availableData = MEGA.available();
  Serial.print("Available data: ");
  Serial.println(availableData, HEX);
            
  
  while(MEGA.available() > 0)
  {    
    BackFromSenityStatus = MEGA.read();
    Serial.print("Received data: ");
    Serial.println(BackFromSenityStatus, DEC);
    LoRa.beginPacket();
    Serial.println("Send Data to Gate LoRa");
    //MEGA.println("Send Data to Gate LoRa");
    LoRa.write(BackFromSenityStatus);
    LoRa.endPacket();
    Serial.println("End Data Sending to Gate LoRa");
    //MEGA.println("End Data Sending to Gate LoRa");
  }
}



void SendCardDataToArduino()
{  
  for(int i = 0; i < sizeof(CardDataFromKapu); i += 1)
  {
      MEGA.write(CardDataFromKapu[i]);
  }
}
  