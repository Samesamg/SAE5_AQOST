#ifndef __ENDNODE_HPP__
#define __ENDNODE_HPP__

#include "Arduino.h"
#include "STM32RTC.h"
#include "STM32LoRaWAN.h"
#include "STM32LowPower.h"
#include "storage.hpp"
#include "DS18B20.h"
#include "definitions.hpp"

#define TEMP_PACKET 0
#define TEMP_BAT_PACKET 1
#define ALARM_PACKET 2

class Endnode
{
public:
    bool init();
    void process();
    void updateTempAlarms(); 
    void updateLoraInfos();
    void enterDeepSleep();
    Endnode();

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