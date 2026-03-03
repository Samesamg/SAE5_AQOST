#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "storage.hpp"
#include "OneWireEEPROM.hpp"

class serialRequest {
    public:

    String message;
    String hexNumbers;
    uint8_t writeBuffer[16];
    uint8_t readBuffer[16];
    bool devicePresent=0;
    bool readFault=0;
    bool writeFault=0;

    serialRequest();
    void param(String msg);
    void AppKey();
    void DevEUI();
    void AppEUI();
    private:
    Storage _storage;


};