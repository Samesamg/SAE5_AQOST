#include "OneWireEEPROM.hpp"

//AT21CS01 ic 

#define OPCODE_MANUFACTURER_ID_READ 0xC0
#define OPCODE_EEPROM_ACCESS 0xA0
#define EEPROM_READ 0x01
#define EEPROM_WRITE 0x00

//constructor
OneWireEeprom::OneWireEeprom(uint8_t eepromIoPin, uint8_t deviceAddress)
{
    _eepromIoPin=eepromIoPin;
    _deviceAddress=(deviceAddress<<1)&0x0E; //device address on 3 bits [3;1], bit 0 is the read/write bit 
}

//public methods
bool OneWireEeprom::init()
{ 
    noInterrupts();
    sdioInit();
    //RESET SEQUENCE
    sdioReleaseLine();
    delayMicroseconds(200);
    sdioWriteLow();
    delayMicroseconds(500);
    sdioReleaseLine();
    delayMicroseconds(20);

    //DISCOVERY SEQUENCE
    sdioWriteLow();
    delayMicroseconds(1);
    sdioReleaseLine();
    delayMicroseconds(2);
    bool devicePresent = !sdioRead();
    delayMicroseconds(21);
    sdioReleaseLine();
    interrupts();
    return devicePresent;
}

bool OneWireEeprom::readManufacturerId(uint8_t* dataBuffer)
{
    bool FAULT=0, ACK=1;

    startStopCondition();
    
    if (sendByte(OPCODE_MANUFACTURER_ID_READ | _deviceAddress | EEPROM_READ)==0)  //ACK by client
    {
        for(uint8_t idx=0; idx<3;idx++)
        {
            ACK=(idx<2)?1:0; //NACK on last byte
            
            dataBuffer[idx]=receiveByte(ACK);
        }
    }
    else 
    {
        FAULT|=1;
    }

    startStopCondition();
    return FAULT;
}

bool OneWireEeprom::readMemory(uint8_t* dataBuffer, uint8_t address, uint8_t length)
{
    bool FAULT=0, ACK=1;

    startStopCondition();

    //SET ADDRESS (DUMMY WRITE)
    FAULT|=sendByte(OPCODE_EEPROM_ACCESS | _deviceAddress | EEPROM_WRITE);//ACK by client
    FAULT|=sendByte(address);//address on 7 bits

    //RESTART 
    startStopCondition();

    //READ AT SET ADDRESS
    FAULT|=sendByte(OPCODE_EEPROM_ACCESS | _deviceAddress | EEPROM_READ);

    if (FAULT==0)  
    {
        for(uint8_t idx; idx<length; idx++)
        {
            ACK=(idx<(length-1))?1:0;  //NACK on last byte
            dataBuffer[idx]=receiveByte(ACK);
        }
    }

    startStopCondition();

    return FAULT;
}

bool OneWireEeprom::writeMemory(uint8_t* dataBuffer, uint8_t address, uint8_t length)
{
    bool FAULT=0, ACK=1;

    length = (length>8)?8:length;  //max page size = 8

    startStopCondition();

    //SET ADDRESS 
    FAULT|=sendByte(OPCODE_EEPROM_ACCESS | _deviceAddress | EEPROM_WRITE);//ACK by client
    FAULT|=sendByte(address&0x7F);//address on 7 bits

    if(FAULT==0)
    {
        for(uint8_t idx=0; idx<length; idx++)
        {
            FAULT|=sendByte(dataBuffer[idx]);
        }
        startStopCondition();
        delay(8); //wait for eeprom to write 
        
    }
    else 
    {
        startStopCondition();
    }    

    return FAULT;
}

//private methods

uint8_t OneWireEeprom::receiveByte(bool ACK)
{
    uint8_t receivedByte=0x00;
    noInterrupts();

    for(uint8_t i=0;i<8;i++)
    {
        receivedByte |= readBit()<<(7-i);
    }

    writeBit(!ACK); //ACK=0 ; NACK=1
    interrupts();
    return receivedByte;
}

bool OneWireEeprom::sendByte(uint8_t byteToSend)  //return ACK(0) or NACK(1)
{
    noInterrupts();
    bool ACK, bit;
    for(uint8_t i=0;i<8;i++)
    {
        bit=0x80&(byteToSend<<i);
        writeBit(bit);
    }

    ACK=readBit();

    interrupts();
    return(ACK);
}

void OneWireEeprom::writeBit(bool bit)
{ //  1us<Tlow1<2us  ;  6us<Tlow0<16us
    if(bit)
    {
        sdioWriteLow();
        delayMicroseconds(1);
        sdioReleaseLine();
        delayMicroseconds(14); 
    }
    else 
    {
        sdioWriteLow();
        delayMicroseconds(10);
        sdioReleaseLine();
        delayMicroseconds(5); 
    }
}

bool OneWireEeprom::readBit()
{
    bool bit;
    sdioWriteLow();
    delayMicroseconds(1);  // 1us<Trd<2us
    sdioReleaseLine();
    delayMicroseconds(1);  // Tmrs<2us
    bit = sdioRead();
    delayMicroseconds(9);
    return bit;
}


void OneWireEeprom::sdioWriteLow()
{
  LL_GPIO_SetPinMode(_gpio, _pin, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_ResetOutputPin(_gpio, _pin);
}

void OneWireEeprom::sdioInit()
{
  _pin=STM_LL_GPIO_PIN(_eepromIoPin);  
  _gpio = set_GPIO_Port_Clock(STM_PORT(_eepromIoPin));
  LL_GPIO_SetPinSpeed(_gpio, _pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(_gpio, _pin, LL_GPIO_PULL_NO);
  LL_GPIO_SetPinOutputType(_gpio, _pin, LL_GPIO_OUTPUT_OPENDRAIN);
  
}

void OneWireEeprom::sdioReleaseLine()
{
  LL_GPIO_SetPinMode(_gpio, _pin, LL_GPIO_MODE_INPUT);
}

bool OneWireEeprom::sdioRead()
{
  return LL_GPIO_IsInputPinSet(_gpio, _pin);
}

void OneWireEeprom::startStopCondition()
{
    sdioReleaseLine();
    delayMicroseconds(200);
}