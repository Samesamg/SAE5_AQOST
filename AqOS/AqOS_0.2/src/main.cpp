/*
AQOST - AqOS_0.1 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#define TEMPBATSEND_TIMEOUT 20  //86400s = 24h
#define TEMPSEND_TIMEOUT 10  //60s*15 = 15min

#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "storage.hpp"
#include "DS18B20.h"
#include "STM32RTC.h"
#include "STM32LowPower.h"

STM32LoRaWAN modem;
STM32RTC& rtc = STM32RTC::getInstance();
HardwareSerial Serial1(PB7, PB6); //(RX, TX)
Storage storage(PC1,0x00); //(EEPROM IO PIN, ADDRESS)
DS18B20 temp_sensor(PB5);  //(DS18B20 IO PIN)

void mainProcessInit();
void mainProcess();
bool sendPacket(bool packetType, uint8_t* payload);

void rtcAlarmCallbackA(void* data);
void rtcAlarmCallbackB(void* data);

bool tempSendFlag=0;
bool batTempSendFlag=0;

bool ledState=0;

void setup() 
{
  Serial1.begin(115200);
  Serial1.println("Start");

  pinMode(PB15, OUTPUT);
  digitalWrite(PB15, 1);

  storage.begin();

  uint8_t devEui[8] = {0x00, 0x80, 0xE1, 0x15, 0x05, 0x46, 0xF8, 0xB7};;
  //modem.getDevEui((uint64_t*)devEui); //Device EUI baked in STM32 (on startup)
  storage.writeToMemory(devEui,Storage::Slots::DEV_EUI);
  uint8_t appEui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6, 0x29};
  storage.writeToMemory(appEui,Storage::Slots::APP_EUI);
  uint8_t appKey[16] = {0xB2, 0x06, 0x9D, 0x52, 0xB8, 0xC9, 0x7F, 0x1E, 0xD3, 0x27, 0x7A, 0x12, 0x8F, 0xF1, 0x1A, 0xA7};//"F0EA82A2D56E0BFC2E5B6E664A1302B3"; //Application key provided by server
  storage.writeToMemory(appKey,Storage::Slots::APP_KEY);

  mainProcessInit();

}

void mainProcessInit() //init for main process (no debug)
{
  modem.begin(EU868);  //init SubGhz
  rtc.begin();
  LowPower.begin();
  
  uint8_t devEui[8];
  storage.readFromMemory(devEui, Storage::Slots::DEV_EUI);
  modem.setDevEui(devEui);
  uint8_t AppEui[8];
  storage.readFromMemory(AppEui, Storage::Slots::APP_EUI);
  modem.setAppEui(AppEui);
  uint8_t AppKey[16];
  storage.readFromMemory(AppKey, Storage::Slots::APP_KEY);
  modem.setAppKey(AppKey);

  bool connected=0;
  connected=modem.joinOTAA();

  while (!connected)
  {
    connected=modem.joinOTAA();
    LowPower.deepSleep(10000);  //Join attempt each 10s
  }

  rtc.attachInterrupt(rtcAlarmCallbackA, STM32RTC::ALARM_A);
  rtc.setAlarmEpoch(rtc.getEpoch() + TEMPSEND_TIMEOUT, STM32RTC::MATCH_SS, STM32RTC::ALARM_A);

  rtc.attachInterrupt(rtcAlarmCallbackB, STM32RTC::ALARM_B);
  rtc.setAlarmEpoch(rtc.getEpoch() + TEMPBATSEND_TIMEOUT, STM32RTC::MATCH_SS, STM32RTC::ALARM_B);
  

}

void loop() 
{
  mainProcess();
  LowPower.deepSleep(1000);
}

void mainProcess()
{
  if(batTempSendFlag)
  {
    uint8_t data[4];
    temp_sensor.getTempRaw(data);
    //TODO : analog read bat
    sendPacket(1,data);
    batTempSendFlag=0;
  }
  else if(tempSendFlag)
  {
    uint8_t data[2];
    temp_sensor.getTempRaw(data);
    sendPacket(0,data);
    tempSendFlag=0;
    digitalWrite(PB15, ledState);
    ledState=!ledState;
  }
  //TODO : check if temp alarms

}

bool sendPacket(bool packetType, uint8_t* payload) //packetType = 0 (Temp), packetType = 1 (Temp+Bat)
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

  return (modem.endPacket() == (int)payloadLength); //packet sent
}

void rtcAlarmCallbackA(void* data) //Alarm A callback => raise temperature send flag
{
  tempSendFlag=1;

  rtc.setAlarmEpoch(rtc.getEpoch() + TEMPSEND_TIMEOUT, STM32RTC::MATCH_SS, STM32RTC::ALARM_A);
}

void rtcAlarmCallbackB(void* data) //Alarm B callback => raise temperature + battery send flag
{
  batTempSendFlag=1;

  rtc.setAlarmEpoch(rtc.getEpoch() + TEMPBATSEND_TIMEOUT, STM32RTC::MATCH_SS, STM32RTC::ALARM_B);
}

