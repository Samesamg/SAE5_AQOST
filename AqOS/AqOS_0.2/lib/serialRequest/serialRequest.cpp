#include "serialRequest.hpp"
#include "iostream"

String serialRequest::setup(String msg)
{
    if (msg == "/setup")
    {
        Serial1.println("Entrez commande : ");
        while (Serial1.available() == 0) { //available is like a mailbox
            delay(10); 
        }
        cmd = Serial1.readStringUntil('\n');

        if (cmd == "AppKey")
        {
            Serial1.println("Entrez AppKey : ");
            while (Serial1.available() == 0) { //available is like a mailbox
            delay(10); 
            }
            message = Serial1.readStringUntil('\n');
            if (message.length() == 32)
            {
                return message;
            }
            else 
            {
                Serial1.println("Mauvais AppKey");
                return "";
            }
        }

        else if (cmd == "DevEUI")
        {
            Serial1.println("Entrez DevEUI : ");
            while (Serial1.available() == 0) { //available is like a mailbox
            delay(10); 
            }
            message = Serial1.readStringUntil('\n');
            if (message.length() == 16)
            {
                return message;
            }
            else 
            {
                Serial1.println("Mauvais DevEUI");
                return "";
            }
        }

        else if (cmd == "AppEUI")
        {
            Serial1.println("Entrez AppEUI : ");
            while (Serial1.available() == 0) { //available is like a mailbox
            delay(10); 
            }
            message = Serial1.readStringUntil('\n');
            if (message.length() == 16)
            {
                return message;
            }
            else 
            {
                Serial1.println("Mauvais AppEUI");
                return "";
            }
        }

        else 
        {
            Serial1.println("Mauvaise commande");
            return "";
        }
    }
}
