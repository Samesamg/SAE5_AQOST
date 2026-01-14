/*
AQOST - AqOS_0.1 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "storage.hpp"
#include "OneWireEEPROM.hpp"


//STM32LoRaWAN modem;
HardwareSerial Serial1(PB7, PB6); //(RX, TX)
Storage storage(PC1,0x00); //PUT EEPROM IO PIN AND ADDRESS HERE
//OneWireEeprom eeprom(PC1,0x00); //PUT EEPROM IO PIN AND ADDRESS HERE


static const unsigned long TX_INTERVAL = 60000; // Interval between TX in ms
unsigned long last_tx = 0;
bool connected = 0;
uint8_t readBuffer[8];
uint8_t writeBuffer[8];

void setup() 
{
  writeBuffer[0]=0x01;
  writeBuffer[1]=0x02;
  writeBuffer[2]=0x03;
  writeBuffer[3]=0x04;
  writeBuffer[4]=0x05;
  writeBuffer[5]=0x06;
  writeBuffer[6]=0x07;
  writeBuffer[7]=0x08;

  //pinMode(PB15, OUTPUT);
  //digitalWrite(PB15, HIGH);  //Enable Led
  bool devicePresent=0;
  bool readFault=0;
  bool writeFault=0;

  Serial1.begin(115200);
  Serial1.println("Start");
  
  devicePresent=storage.init();
  //writeFault|=storage.writeToMemory(writeBuffer, Storage::Slots::APP_EUI);
  readFault|=storage.readFromMemory(readBuffer, Storage::Slots::APP_EUI);

  Serial1.println("Next");
  //storage.writeToMemory(&test, Storage::Slots::APPEUI);
  //devicePresent=eeprom.init();
  //writeFault=eeprom.writeMemory(writeBuffer, 0x08, 8);
  //readFault=eeprom.readMemory(readBuffer, 0x08, 8);
  //readFault=eeprom.readManufacturerId(readBuffer);
  //delay(100);
  //readFault=eeprom.readMemory(readBuffer, 0x03, 1);
  //readFault=eeprom.readMemory(readBuffer, 0x04, 1);
  //readFault=eeprom.readMemory(&readBuffer[2], 0x05, 1);

  //delay(50);
  //Serial1.println(test);
  Serial1.println(devicePresent);
  Serial1.println(writeFault);
  Serial1.println(readFault);
  
  Serial1.print(readBuffer[0]);
  Serial1.print(readBuffer[1]);
  Serial1.print(readBuffer[2]);
  Serial1.print(readBuffer[3]);
  Serial1.print(readBuffer[4]);
  Serial1.print(readBuffer[5]);
  Serial1.print(readBuffer[6]);
  Serial1.println(readBuffer[7]);
  //modem.begin(EU868);  //init SubGhz
/*
  String DevEUI = modem.deviceEUI(); //Device EUI baked in STM32 
  String AppKey = "B2069D52B8C97F1ED3277A128FF11AA7";  //Application key provided by server
  String AppEUI = "000000000000E629";  //Application EUI provided by server
  Serial1.println("DevEUI : " + DevEUI);
  Serial1.println("AppEUI : " + AppEUI);   
  Serial1.println("AppKey : " + AppKey); 
//
//
connected = modem.joinOTAA(AppEUI, AppKey, DevEUI); 
//
//
//if (connected) 
//{
//  Serial1.println("Joined");  //Connection established - PB5 turns on
//  pinMode(PB5, OUTPUT);
//} 
//else 
//{
//  Serial1.println("Join failed"); 
//  while (true) {};
//}
*/
}

/*
void send_packet() 
{
  char payload[27] = { 0 }; 
  sprintf(payload,"Hello World !");  //Format data into payload buffer

  modem.setPort(3);  //Set TX port
  modem.beginPacket(); 
  modem.write(payload, strlen(payload));

  if (modem.endPacket() == (int)strlen(payload)) 
  {
    Serial1.println("Sent packet");
  } 
  else 
  {
    Serial1.println("Failed to send packet");
  }
}
*/
void loop() 
{
  /*
  if (!last_tx || millis() - last_tx > TX_INTERVAL) 
  {
    send_packet();
    last_tx = millis();
  }
    */
  
  //Serial1.println((bool)eeprom.readManufacturerId(buffer));
  //delay(1000);
}
