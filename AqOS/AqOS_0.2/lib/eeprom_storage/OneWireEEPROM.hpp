#ifndef __ONEWIRE_EEPROM_HPP__
#define __ONEWIRE_EEPROM_HPP__

#include "Arduino.h"

class OneWireEeprom
{
public: 
    OneWireEeprom(){};
    bool init(uint8_t eepromIoPin, uint8_t deviceAddress);
    bool readManufacturerId(uint8_t* dataBuffer);
    bool readMemory(uint8_t* dataBuffer, uint8_t address, uint8_t length);
    bool writeMemory(uint8_t* dataBuffer, uint8_t address, uint8_t length);
    
private: 
    uint8_t receiveByte(bool ACK);
    bool sendByte(uint8_t byteToSend);
    bool readBit();
    void writeBit(bool bit);
    void startStopCondition();
    //sdio GPIO control
    void sdioWriteLow();
    void sdioReleaseLine();
    void sdioInit();
    bool sdioRead();
    uint8_t _eepromIoPin;
    uint8_t _deviceAddress;
    uint32_t _pin;
    GPIO_TypeDef *_gpio;
};

#endif