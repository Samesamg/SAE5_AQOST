#include "storage.hpp"

Storage::Storage(uint8_t eepromIoPin, uint8_t deviceAddress): _eeprom(eepromIoPin, deviceAddress)
{
    _writeIdx=0;
    _readIdx=0;
}

bool Storage::init()
{
    bool DEVICE_PRESENT=0;
    DEVICE_PRESENT|=_eeprom.init();
    return DEVICE_PRESENT;
}

bool Storage::writeToMemory(uint8_t* dataBuffer, Slots memorySlot)
{
    bool FAULT=0;
    switch (memorySlot) 
    {
        case Storage::Slots::DEV_EUI :        
        case Storage::Slots::APP_EUI :
            FAULT|=_eeprom.writeMemory(dataBuffer, memorySlot, 8);
            break;

        case Storage::Slots::APP_KEY : 
            FAULT|=_eeprom.writeMemory(dataBuffer, memorySlot, 0x08);  //first 8 Bytes
            FAULT|=_eeprom.writeMemory(&dataBuffer[8], memorySlot+0x08, 0x08);  //last 8 Bytes
            break;
        
        case Storage::Slots::TEMP_SENSOR_DATA : 
            uint8_t address; 
            fifoGetNextAddress(0, &address);
            FAULT|=_eeprom.writeMemory(dataBuffer, address, 2);
            break;

        default :
            FAULT|=1;
            break;
    }
    return FAULT;
}

bool Storage::readFromMemory(uint8_t* dataBuffer, Slots memorySlot)
{
    bool FAULT=0;
    switch (memorySlot) 
    {
        case Storage::Slots::DEV_EUI :        
        case Storage::Slots::APP_EUI :
            FAULT|=_eeprom.readMemory(dataBuffer, memorySlot, 8);
            break;
        
        case Storage::Slots::APP_KEY : 
            FAULT|=_eeprom.readMemory(dataBuffer, memorySlot, 8);  //first 8 Bytes
            FAULT|=_eeprom.readMemory(&dataBuffer[8], memorySlot+0x08, 8);  //last 8 Bytes
            break;

        case Storage::Slots::FIFO_IDX : 
            FAULT|=_eeprom.readMemory(&_writeIdx, memorySlot, 2);
            break;
        
        case Storage::Slots::TEMP_SENSOR_DATA : 
            uint8_t address; 
            fifoGetNextAddress(1, &address);
            FAULT|=_eeprom.readMemory(dataBuffer, address, 2);
            break;
        
        default :
            FAULT|=1;
            break;
    }
    return FAULT;
}

bool Storage::getWriteBufferIdx(uint8_t* data)
{
    bool FAULT=0;
    FAULT|=_eeprom.readMemory(data,FIFO_IDX, 1);
}

bool Storage::fifoGetNextAddress(bool writeRead, uint8_t* address)  //TODO Circular buffer (save index at 0x20)
{
    bool FAULT=0;
    if(writeRead) //Read
    {
        _readIdx+=2;
        _readIdx=(_readIdx>94)?0:_readIdx;  //Has index overflowed ? (94 Bytes is the available size in EEPROM)
        *address=TEMP_SENSOR_DATA+_readIdx;  //Base address + index
    }
    else    //Write
    {
        _writeIdx+=2;
        _writeIdx=(_writeIdx>94)?0:_writeIdx;  //Has index overflowed ? (94 Bytes is the available size in EEPROM)
        *address=TEMP_SENSOR_DATA+_writeIdx;  //Base address + index
        FAULT|=_eeprom.writeMemory(&_writeIdx,FIFO_IDX, 1);
    }
}