/*
AQOST - AqOS_0.1 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "storage.hpp"
#include "DS18B20.h"
#include "STM32RTC.h"


STM32LoRaWAN modem;
HardwareSerial Serial1(PB7, PB6); //(RX, TX)
Storage storage(PC1,0x00); //PUT EEPROM IO PIN AND ADDRESS HERE
DS18B20 temp_sensor(PB5);

void processInit();

void setup() 
{
  Serial1.begin(115200);
  Serial1.println("Start");

  storage.init();

  uint8_t devEui[8];
  modem.getDevEui((uint64_t*)devEui); //Device EUI baked in STM32 (on startup)
  storage.writeToMemory(devEui,Storage::Slots::DEV_EUI);
  uint8_t appEui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6, 0x29};
  storage.writeToMemory(appEui,Storage::Slots::APP_EUI);
  uint8_t appKey[16] = {0xB2, 0x06, 0x9D, 0x52, 0xB8, 0xC9, 0x7F, 0x1E, 0xD3, 0x27, 0x7A, 0x12, 0x8F, 0xF1, 0x1A, 0xA7};//"F0EA82A2D56E0BFC2E5B6E664A1302B3"; //Application key provided by server
  storage.writeToMemory(appKey,Storage::Slots::APP_KEY);

  uint8_t readBuffer[16];

  modem.begin(EU868);  //init SubGhz

  storage.readFromMemory(readBuffer, Storage::Slots::DEV_EUI);
  modem.setDevEui((uint64_t)readBuffer);
  storage.readFromMemory(readBuffer, Storage::Slots::APP_EUI);
  modem.setAppEui((uint64_t)readBuffer);
  storage.readFromMemory(readBuffer, Storage::Slots::APP_KEY);
  modem.setAppKey()

  processInit();

}

void processInit() //init for main process (no debug)
{

  bool connected=modem.joinOTAA();

  if (connected) 
  {
    Serial1.println("Joined"); 
  } 
  else 
  {
    Serial1.println("Join failed"); 
    while (true) {};
  }
}

void debugInit()
{
  Serial1.begin(115200);
  Serial1.println("Start");
}

void sendPacket(bool packetType, uint8_t* payload) //packetType = 0 (Temp), packetType = 1 (Temp+Bat)
{
  uint8_t payloadLength, txport;
  if(packetType) //temp+bat
  {
    payloadLength=4;
    txport=3;
  }
  else  //temp only
  {
    payloadLength=2;
    txport=2;
  }

  modem.setPort(txport);  //Set TX port
  modem.beginPacket(); 
  modem.write(payload, payloadLength);

  if (modem.endPacket() == (int)payloadLength) 
  {
    Serial1.println("Sent packet");
  }
  else 
  {
    Serial1.println("Failed to send packet");
  }
}

void loop() 
{
  /*
  if (!last_tx || millis() - last_tx > TX_INTERVAL) 
  {
    uint8_t tempBuffer[2];
    temp_sensor.getTempRaw(tempBuffer);
    send_packet(0, tempBuffer);
    last_tx = millis();
  }
  */
}