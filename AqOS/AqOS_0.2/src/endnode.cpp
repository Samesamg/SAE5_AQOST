#include "endnode.hpp"

bool Endnode::ledState;  

//RTC alarm A used by lowPower (to wakeup from deepSleep)
//RTC alarm B used by STM32LoraWan (for timeouts)

Endnode::Endnode() : _modem(), _tempSensor(TEMP_SENSOR_PIN)
{}

bool Endnode::init()
{
    LowPower.begin();
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
    

    return (!connected);
}

void Endnode::process()
{
    if ((bool)_tempSensor.hasAlarm())
    {
        uint8_t data[2];
        _tempSensor.getTempRaw(data);
        sendPacket(ALARM_PACKET, data);
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
            sendPacket(TEMP_BAT_PACKET, data);
            _txCounter=0;
        }
        else //Temp send 
        {
            uint8_t data[2];
            _tempSensor.getTempRaw(data);
            sendPacket(TEMP_PACKET, data);
            _txCounter++;
        }
    }
    enterDeepSleep();
}

void Endnode::updateTempAlarms()
{
    uint8_t Alarm;
    if (!Storage::getInstance().readFromMemory(&Alarm, Storage::Slots::TEMP_ALARM_HIGH))  //No read Fault
    {
        _tempSensor.setAlarmHigh((int8_t)Alarm);
    }
    if (!Storage::getInstance().readFromMemory(&Alarm, Storage::Slots::TEMP_ALARM_LOW))    //No read Fault
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

void Endnode::enterDeepSleep()  //Enter deepSleep for WAKEUP_TIMEOUT amount of time
{
    //Put Gpios to analog mode with no pull up/down
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA0), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA2), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA3), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA9), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA10), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA13), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA14), LL_GPIO_MODE_ANALOG);

    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA0), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA2), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA3), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA9), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA10), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA13), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOA_BASE, STM_LL_GPIO_PIN(PA14), LL_GPIO_PULL_NO);
    
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB0), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB3), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB4), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB5), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB6), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB7), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB9), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB10), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB13), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB14), LL_GPIO_MODE_ANALOG);

    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB0), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB3), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB4), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB5), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB6), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB7), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB9), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB10), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB13), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOB_BASE, STM_LL_GPIO_PIN(PB14), LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOC_BASE, STM_LL_GPIO_PIN(PC0), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode((GPIO_TypeDef *)GPIOC_BASE, STM_LL_GPIO_PIN(PC1), LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOC_BASE, STM_LL_GPIO_PIN(PC13), LL_GPIO_PULL_NO);
    LL_GPIO_SetPinPull((GPIO_TypeDef *)GPIOC_BASE, STM_LL_GPIO_PIN(PC14), LL_GPIO_PULL_NO);

    DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
    LowPower.deepSleep(WAKEUP_TIMEOUT);
}

void Endnode::blinkAlarmCallback(void* data) //Alarm A callback for led blink
{
    ledState=!ledState;
    digitalWrite(LED_PIN, ledState);
    STM32RTC::getInstance().setAlarmEpoch(STM32RTC::getInstance().getEpoch() + 1, STM32RTC::MATCH_SS, STM32RTC::ALARM_A);
}