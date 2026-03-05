/*
AQOST - AqOS_0.2 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#define WAKEUP_PERIOD 10000 //ms

#include <Arduino.h>
#include "storage.hpp"
#include "STM32LowPower.h"
#include "endnode.hpp"

HardwareSerial Serial1(PB7, PB6); //(RX, TX)
Endnode endnode(Serial1);


void setup() 
{
  Serial1.begin(115200);
  Serial1.println("Start");

  LowPower.begin();

  Storage::getInstance().setIoPin(PC1);
  Storage::getInstance().setAddress(0x00);
  Storage::getInstance().init();

  uint8_t devEui[8] = {0x00, 0x80, 0xE1, 0x15, 0x05, 0x46, 0xF8, 0xB7};;
  //modem.getDevEui((uint64_t*)devEui); //Device EUI baked in STM32 (on startup)
  Storage::getInstance().writeToMemory(devEui,Storage::Slots::DEV_EUI);
  uint8_t appEui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6, 0x29};
  Storage::getInstance().writeToMemory(appEui,Storage::Slots::APP_EUI);
  uint8_t appKey[16] = {0xB2, 0x06, 0x9D, 0x52, 0xB8, 0xC9, 0x7F, 0x1E, 0xD3, 0x27, 0x7A, 0x12, 0x8F, 0xF1, 0x1A, 0xA7};//"F0EA82A2D56E0BFC2E5B6E664A1302B3"; //Application key provided by server
  Storage::getInstance().writeToMemory(appKey,Storage::Slots::APP_KEY);

  endnode.init();

}

void loop() 
{
  endnode.process();
  LowPower.deepSleep(WAKEUP_PERIOD);
}
