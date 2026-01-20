#include <Arduino.h>
#include "STM32LoRaWAN.h"

class serialRequest {
    public:
    String cmd;
    String message;
    String setup(String msg);

};