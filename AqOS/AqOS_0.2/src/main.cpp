/*
AQOST - AqOS_0.2 - 17/03/2025
Complete OS : periodic temperature/battery transmission ; Low power process ; Serial CLI for settings
*/

#include <Arduino.h>
#include "endnode.hpp"
#include "maintenance_cli.hpp"
#include "definitions.hpp"

Endnode endnode;
MaintenanceCLI cli;

void setup() 
{
  Storage::getInstance().setIoPin(EEPROM_PIN);
  Storage::getInstance().setAddress(0x00);
  Storage::getInstance().init();

  pinMode(MAINTENANCE_PIN, INPUT_FLOATING);

  if(digitalRead(MAINTENANCE_PIN)==0)  //Maintenance CLI mode
  {
    cli.init();
    while(1)
    {
      cli.process();
    }
  }
  else //normal mode
  {
    if(endnode.init())  //init failed
    {
      do 
      {
        endnode.enterDeepSleep();
      }
      while(endnode.init());  //retry in WAKEUP_TIMEOUT
    }
  }
}

void loop() 
{
  endnode.process();  //process flags / enter deepSleep for WAKEUP_TIMEOUT
}
