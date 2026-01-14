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
        
        case Storage::Slots::SINGLE_BYTE_DATA : 
            uint8_t address; 
            FAULT |= fifoGetNextAddress(0, &address);
            if (!FAULT)
                FAULT|=_eeprom.writeMemory(dataBuffer, address, 0x01);
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
            FAULT|=_eeprom.readMemory(dataBuffer, memorySlot, 0x08);  //first 8 Bytes
            FAULT|=_eeprom.readMemory(&dataBuffer[8], memorySlot+0x08, 0x08);  //last 8 Bytes
            break;
        
        case Storage::Slots::SINGLE_BYTE_DATA : 
            uint8_t address; 
            FAULT |= fifoGetNextAddress(1, &address);
            if (!FAULT)
                FAULT|=_eeprom.readMemory(dataBuffer, address, 0x01);
            break;
        
        default :
            FAULT|=1;
            break;
    }
    return FAULT;
}

bool Storage::fifoGetNextAddress(bool writeRead, uint8_t* address)
{
    bool FAULT=0;
    if(writeRead) //Read
    {
        _readIdx++;
        FAULT|=(_readIdx>96)?1:0;  //Has index overflowed ? (96 Bytes is the available size in EEPROM)
        if (!FAULT)
            *address=SINGLE_BYTE_DATA+_readIdx;  //Base address + index
    }
    else    //Write
    {
        _writeIdx++;
        FAULT|=(_writeIdx>96)?1:0;  //Has index overflowed ? (96 Bytes is the available size in EEPROM)
        if (!FAULT)
            *address=SINGLE_BYTE_DATA+_writeIdx;  //Base address + index
    }
    return FAULT;
}