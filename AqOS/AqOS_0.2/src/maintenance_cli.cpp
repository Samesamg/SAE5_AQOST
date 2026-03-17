#include "maintenance_cli.hpp"

MaintenanceCLI::MaintenanceCLI():_serial1(UART_RX_PIN, UART_TX_PIN)
{}

void MaintenanceCLI::init()
{
    _serial1.begin(115200);
    _serial1.println("\n ~~~~~~ AqOS maintenance CLI  ~~~~~~ \n\n");
}

void MaintenanceCLI::process()
{
    if(_serial1.available())
    {
        _message = _serial1.readStringUntil('\n');
        
        if (_message == "set deveui")
        {
            setDevEUI();
        }
        else if (_message == "set unique deveui")
        {
            uint64_t devEUI = (uint64_t)LL_FLASH_GetSTCompanyID() << (5 * 8) | (uint64_t)LL_FLASH_GetDeviceID() << (4 * 8) | (uint64_t)LL_FLASH_GetUDN();
            Storage::getInstance().writeToMemory((uint8_t*)&devEUI, Storage::Slots::DEV_EUI);
            _serial1.print("\nUnique deveui set !");
        }
        else if (_message == "set appeui")
        {
            setAppEUI();
        }
        else if (_message == "set appkey")
        {
            setAppKey();
        }
        else if (_message == "get device info") 
        {
            getDeviceInfo();
        }
        else if (_message == "set alarm high")
        {
            setTempAlarm(1);
        }
        else if (_message == "set alarm low")
        {
            setTempAlarm(0); 
        }
        else if (_message == "help")
        {
            _serial1.println("Command list : ");
            _serial1.println("set deveui");
            _serial1.println("set unique deveui");
            _serial1.println("set appeui");
            _serial1.println("set appkey");
            _serial1.println("set alarm high");
            _serial1.println("set alarm low");
            _serial1.println("get device info"); 
            _serial1.println("reset"); 
        }
        else if (_message == "reset")
        {
            NVIC_SystemReset();
        }
        else 
        {
            _serial1.println("Unknown command, type 'help' to see the available commands.");
        }
    }
    delay(10);
}

void MaintenanceCLI::setAppKey()
{
    uint8_t writeBuffer[16];
    _serial1.println("Enter AppKey : ");
    while (_serial1.available() == 0) { 
        delay(10); 
    }
    _message = _serial1.readStringUntil('\n');
    if (_message.length() == 32)
        {       
            for (int i=0;i<16;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa qu'on vient convertir en décimal
            {
                String charBuffer = String(_message[i*2]) + String(_message[i*2 + 1]) ;
                writeBuffer[i] = strtoul(charBuffer.c_str(),0,16);
            }
            Storage::getInstance().writeToMemory(writeBuffer, Storage::Slots::APP_KEY);
            _serial1.println("AppKey set !");
        }
        else 
        {
            _serial1.println("Wrong AppKey");
        }
}

void MaintenanceCLI::setDevEUI()
{
    uint8_t writeBuffer[8];
    _serial1.println("Enter DevEUI : ");
    while (_serial1.available() == 0) { 
        delay(10); 
    }
    _message = _serial1.readStringUntil('\n');
    if (_message.length() == 16)
    {
        for (int i=0;i<8;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa
        {
            String charBuffer = String(_message[i*2]) + String(_message[i*2 + 1]) ;
            writeBuffer[i] = strtoul(charBuffer.c_str(),0,16);
        }
        Storage::getInstance().writeToMemory(writeBuffer, Storage::Slots::DEV_EUI);
        _serial1.println("New DevEUI saved ! ");
    }
    else 
    {
        _serial1.println("Wrong DevEUI");
    }
}

void MaintenanceCLI::setAppEUI()
{
    uint8_t writeBuffer[8];
    _serial1.println("Enter AppEUI : ");
    while (_serial1.available() == 0) { 
        delay(10); 
    }
    _message = _serial1.readStringUntil('\n');
    if (_message.length() == 16)
    {
        for (int i=0;i<8;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa
        {
            String charBuffer = String(_message[i*2]) + String(_message[i*2 + 1]) ;
            writeBuffer[i] = strtoul(charBuffer.c_str(),0,16);
        }
        Storage::getInstance().writeToMemory(writeBuffer, Storage::Slots::APP_EUI);
        _serial1.println("New AppEUI saved !");
    }
    else 
    {
        _serial1.println("Wrong AppEUI");
    }
}

void MaintenanceCLI::getDeviceInfo()
{
    uint8_t readBuffer[8];
    int8_t alarmValue;
    char charToPrint[2];

    _serial1.print("\nDevEUI : ");
    Storage::getInstance().readFromMemory(readBuffer, Storage::Slots::DEV_EUI);
    for(uint8_t i=0;i<8;i++)
    {
        sprintf(charToPrint, "%02x", readBuffer[i]);
        _serial1.print(charToPrint);
    } 

    _serial1.print("\nAppEUI : ");
    Storage::getInstance().readFromMemory(readBuffer, Storage::Slots::APP_EUI);
    for(uint8_t i=0;i<8;i++)
    {
        sprintf(charToPrint, "%02x", readBuffer[i]);
        _serial1.print(charToPrint);
    } 

    _serial1.print("\nAppKey : ");
    Storage::getInstance().readFromMemory(readBuffer, Storage::Slots::APP_KEY);
    for(uint8_t i=0;i<16;i++)
    {
        sprintf(charToPrint, "%02x", readBuffer[i]);
        _serial1.print(charToPrint);
    } 

    _serial1.print("\nTemperature alarm low : ");
    Storage::getInstance().readFromMemory((uint8_t*)&alarmValue, Storage::Slots::TEMP_ALARM_LOW);
    _serial1.print(alarmValue);
    _serial1.print("°C");

    _serial1.print("\nTemperature alarm high : ");
    Storage::getInstance().readFromMemory((uint8_t*)&alarmValue, Storage::Slots::TEMP_ALARM_HIGH);
    _serial1.print(alarmValue);
    _serial1.print("°C");
}

void MaintenanceCLI::setTempAlarm(bool high)
{
    int8_t tempAlarm;
    if(high)
    {
        _serial1.print("Enter temp alarm high (°C): ");
        while (_serial1.available() == 0) 
        { 
        delay(10); 
        }
        _message = _serial1.readStringUntil('\n');
        tempAlarm = _message.toInt();
        if (tempAlarm>=-55 && tempAlarm<=125)
        {
            Storage::getInstance().writeToMemory((uint8_t*)&tempAlarm, Storage::Slots::TEMP_ALARM_HIGH);
            _serial1.print("Temp alarm high set !");
        }   
    }
    else
    {
        _serial1.print("Enter temp alarm low (°C): ");
        while (_serial1.available() == 0) 
        { 
        delay(10); 
        }
        _message = _serial1.readStringUntil('\n');
        tempAlarm = _message.toInt();
        if (tempAlarm>=-55 && tempAlarm<=125)
        {
            Storage::getInstance().writeToMemory((uint8_t*)&tempAlarm, Storage::Slots::TEMP_ALARM_LOW);
            _serial1.print("Temp alarm low set !");
        }   
    }
}