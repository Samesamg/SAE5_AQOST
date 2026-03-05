#include "endnode.hpp"

bool Endnode::ledState;  

//RTC alarm A used by lowPower (to wakeup from deepSleep)
//RTC alarm B used by STM32LoraWan (for timeouts)

Endnode::Endnode(HardwareSerial &Serial1) : _modem(), _tempSensor(TEMP_SENSOR_PIN)
{}

bool Endnode::init()
{
    _modem.begin(EU868); // init SubGhz modem + rtc
    pinMode(LED_PIN, OUTPUT);
    analogReadResolution(12);

    _txCounter=0;

    _tempSensor.setResolution(12);

    updateTempAlarms();
    updateLoraInfos();

    STM32RTC::getInstance().attachInterrupt(blinkAlarmCallback, STM32RTC::ALARM_A); //led blink start
    STM32RTC::getInstance().setAlarmEpoch(STM32RTC::getInstance().getEpoch() + 1, STM32RTC::MATCH_SS, STM32RTC::ALARM_A);

    bool connected = _modem.joinOTAA();
    if (connected)
    {
        STM32RTC::getInstance().detachInterrupt(STM32RTC::ALARM_A);
        ledState=0;
        for(uint8_t i=0;i<10;i++)
        {
            ledState=!ledState;
            digitalWrite(LED_PIN,ledState);
            delay(200);
        }
        digitalWrite(LED_PIN,0);
    }
    else
    {
        STM32RTC::getInstance().detachInterrupt(STM32RTC::ALARM_A);
        digitalWrite(LED_PIN,1);
    }

    return (connected);
}

void Endnode::process()
{
    if ((bool)_tempSensor.hasAlarm())
    {
        uint8_t data[2];
        _tempSensor.getTempRaw(data);
        sendPacket(ALARM_PACKET, data);
        Serial1.println("AlarmSend");
    }

    else if (STM32RTC::getInstance().getEpoch() >= (_prevSendEpoch + SEND_TIMEOUT))  //if current time > prev time + timeout (NO CHOICE BC ALL RTC ALARMS ARE USED)
    {
        _prevSendEpoch = STM32RTC::getInstance().getEpoch();

        if(_txCounter>=COUNTER_RESET) //Temp+bat send
        {
            uint8_t data[4]; // data[0-1]=temp(msb)-temp(lsb) ; data[2-3]=bat(msb)-bat(lsb)
            _tempSensor.getTempRaw(data);
            uint16_t batAdcValue = analogRead(BAT_ADC_PIN);
            data[2] = (batAdcValue >> 8) & 0xFF; // batAdcValue msb
            data[3] = batAdcValue & 0xFF;        // batAdcValue lsb
            Serial1.println("batTempSend");
            sendPacket(TEMP_BAT_PACKET, data);
            _txCounter=0;
        }
        else //Temp send 
        {
            uint8_t data[2];
            _tempSensor.getTempRaw(data);
            Serial1.println("TempSend");
            sendPacket(TEMP_PACKET, data);
            _txCounter++;
        }
    }
}

void Endnode::updateTempAlarms()
{
    uint8_t Alarm;
    if (Storage::getInstance().readFromMemory(&Alarm, Storage::Slots::TEMP_ALARM_HIGH))
    {
        _tempSensor.setAlarmHigh((int8_t)Alarm);
    }
    if (Storage::getInstance().readFromMemory(&Alarm, Storage::Slots::TEMP_ALARM_LOW))    
    {
        _tempSensor.setAlarmLow((int8_t)Alarm);
    }
}

void Endnode::updateLoraInfos()
{
    uint8_t devEui[8];
    if(!Storage::getInstance().readFromMemory(devEui, Storage::Slots::DEV_EUI))
    {
        _modem.setDevEui(devEui);
    }
    uint8_t AppEui[8];
    if(!Storage::getInstance().readFromMemory(AppEui, Storage::Slots::APP_EUI))
    {
        _modem.setAppEui(AppEui);
    }
    uint8_t AppKey[16];
    if(!Storage::getInstance().readFromMemory(AppKey, Storage::Slots::APP_KEY))
    {
        _modem.setAppKey(AppKey);
    }
}

bool Endnode::sendPacket(uint8_t packetType, uint8_t *payload)
{
    uint8_t payloadLength, txport;

    switch (packetType)
    {
    case TEMP_PACKET:
        payloadLength = 2;
        txport = 2;
        break;
    case TEMP_BAT_PACKET:
        payloadLength = 4;
        txport = 3;
        break;
    case ALARM_PACKET:
        payloadLength = 2;
        txport = 4;
        break;
    default:
        break;
    }

    _modem.setPort(txport);
    _modem.beginPacket(); //init tx pointer to start of tx buffer
    _modem.write(payload, payloadLength); //copy payload to tx buffer

    return (_modem.endPacket() == (int)payloadLength); // packet sent
}


void Endnode::blinkAlarmCallback(void* data) //Alarm A callback for led blink
{
    ledState=!ledState;
    digitalWrite(LED_PIN, ledState);
    STM32RTC::getInstance().setAlarmEpoch(STM32RTC::getInstance().getEpoch() + 1, STM32RTC::MATCH_SS, STM32RTC::ALARM_A);
}