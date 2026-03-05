#ifndef __ENDNODE_HPP__
#define __ENDNODE_HPP__

#include "Arduino.h"
#include "STM32RTC.h"
#include "STM32LoRaWAN.h"
#include "storage.hpp"
#include "DS18B20.h"

#define TEMP_PACKET 0
#define TEMP_BAT_PACKET 1
#define ALARM_PACKET 2

#define BAT_ADC_PIN PA10
#define LED_PIN PB15
#define TEMP_SENSOR_PIN PB5

#define SEND_TIMEOUT 30  // 900s = 15min

#define COUNTER_RESET 96  //each 24h

class Endnode
{
public:
    bool init();
    void process();
    void updateTempAlarms();  //TODO store in EEPROM
    void updateLoraInfos();
    Endnode(HardwareSerial &Serial1);

private:
    bool sendPacket(uint8_t packetType, uint8_t* payload);

    static void blinkAlarmCallback(void* data);
    static bool ledState;

    uint32_t _prevSendEpoch;
    uint8_t _txCounter;

    STM32LoRaWAN _modem;
    DS18B20 _tempSensor;
    Storage _storage;

};

#endif