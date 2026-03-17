#ifndef __MAINTENANCE_CLI_HPP__
#define __MAINTENANCE_CLI_HPP__

#include "storage.hpp"
#include "definitions.hpp"

class MaintenanceCLI
{
    public : 
        MaintenanceCLI();
        void init();
        void process();
    private :
        void setAppKey();
        void setAppEUI();
        void setDevEUI();
        void getDeviceInfo();
        void setTempAlarm(bool high);
        HardwareSerial _serial1;
        String _message;
};

#endif