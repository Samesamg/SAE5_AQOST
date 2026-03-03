#include "serialRequest.hpp"
#include "storage.hpp"



serialRequest::serialRequest():_storage(PC1,0x00)
{
    _storage.init();
}

void serialRequest::AppKey()
{
    Serial1.println("Entrez AppKey : ");
    while (Serial1.available() == 0) { 
        delay(10); 
    }
    message = Serial1.readStringUntil('\n');
    if (message.length() == 32)
        {       
            for (int i=0;i<16;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa qu'on vient convertir en décimal
            {
                hexNumbers = String(message[i*2]) + String(message[i*2 + 1]) ;
                writeBuffer[i] = strtoul(hexNumbers.c_str(),0,16);
            }
            writeFault|=_storage.writeToMemory(writeBuffer, Storage::Slots::APP_KEY);
            Serial1.println("AppKey valide ! ");
        }
        else 
        {
            Serial1.println("Mauvais AppKey");
        }
}

void serialRequest::DevEUI()
{
    Serial1.println("Entrez DevEUI : ");
    while (Serial1.available() == 0) { 
        delay(10); 
    }
    message = Serial1.readStringUntil('\n');
    if (message.length() == 16)
    {
        for (int i=0;i<8;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa
        {
            hexNumbers = String(message[i*2]) + String(message[i*2 + 1]) ;
            writeBuffer[i] = strtoul(hexNumbers.c_str(),0,8);
        }
        writeFault|=_storage.writeToMemory(writeBuffer, Storage::Slots::DEV_EUI);
        Serial1.println("DevEUI valide ! ");
    }
    else 
    {
        Serial1.println("Mauvais DevEUI");
    }
}

void serialRequest::AppEUI()
{
    Serial1.println("Entrez AppEUI : ");
    while (Serial1.available() == 0) { 
        delay(10); 
    }
    message = Serial1.readStringUntil('\n');
    if (message.length() == 16)
    {
        for (int i=0;i<8;i++)      // on prend 2 octets par 2 octets, soit 1 nombre hexa
        {
            hexNumbers = String(message[i*2]) + String(message[i*2 + 1]) ;
            writeBuffer[i] = strtoul(hexNumbers.c_str(),0,8);
        }
        writeFault|=_storage.writeToMemory(writeBuffer, Storage::Slots::APP_EUI);
        Serial1.println("AppEUI valide ! ");
    }
    else 
    {
        Serial1.println("Mauvais AppEUI");
    }
}


void serialRequest::param(String msg)
{
    if (msg == "AppKey")
    {
        AppKey();
    }
    else if (msg == "DevEUI")
    {
        DevEUI();
    }
    else if (msg == "AppEUI")
    {
        AppEUI();
    }
    else {
    }
}
