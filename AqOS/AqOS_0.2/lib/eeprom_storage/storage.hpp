#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include "OneWireEEPROM.hpp"

class Storage 
{
public:
    enum Slots
    {
        DEV_EUI=0x00,  //DevEUI (8 bytes) slot starts at address 0x00 (0x00->0x07)(zone 0)
        APP_EUI=0x08,  //AppEUI (8 bytes) slot starts at address 0x08 (0x07->0x0F)(zone 0)
        APP_KEY=0x10,  //APPKEY (16 bytes) slot starts at address 0x10 (0x10->0x1F)(zone 0)
        FIFO_IDX=0x20,  //FIFO index to read from it
        TEMP_SENSOR_DATA=0x22   //Temp sensor values (2 bytes (12 bit)) slot starts at adress 0x20 (zone 1)
    };
    Storage(uint8_t eepromIoPin, uint8_t deviceAddress);
    Storage(){};
    bool getWriteBufferIdx(uint8_t* data);
    bool writeToMemory(uint8_t* dataBuffer, Slots memorySlot);
    bool readFromMemory(uint8_t* dataBuffer, Slots memorySlot);
    bool begin();

private:
    bool fifoGetNextAddress(bool writeRead, uint8_t* address); //0=write ; 1=read
    OneWireEeprom _eeprom;
    uint8_t _writeIdx;
    uint8_t _readIdx;
    uint8_t _eepromIoPin;
    bool _fifoOverflowed;
};

#endif