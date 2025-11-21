/*
AQOST - AqOS_0.1 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "EEPROM.h"  //Emulate EEPROM with flash memory 

STM32LoRaWAN modem;
HardwareSerial Serial1(PB7, PB6); //(RX, TX)

static const unsigned long TX_INTERVAL = 60000; // Interval between TX in ms
unsigned long last_tx = 0;
bool connected = 0;

void setup() 
{
  pinMode(PB10, OUTPUT);
  pinMode(PA9, OUTPUT);
  digitalWrite(PB10, HIGH);  //Enable 5V on dev board
  digitalWrite(PA9, HIGH);  //Enable 3V3 on dev board
  EEPROM.put(0, 24); //write to flash memory (address index, 8bit data)
  int test=0;
  EEPROM.get(0, test); //read flash memory (address index, 8bit data)

  Serial1.begin(115200);
  Serial1.println("Start");


  Serial1.println(test);

  modem.begin(EU868);  //init SubGhz

 // String DevEUI = modem.deviceEUI(); //Device EUI baked in STM32 
 // String AppKey = "A4E39A0DAF0B9A7FA10C8F2675F325E5";  //Application key provided by server
 // String AppEUI = "000000000000E5DD";  //Application EUI provided by server
 // Serial1.println("DevEUI : " + DevEUI);
 // Serial1.println("AppEUI : " + AppEUI);   
 // Serial1.println("AppKey : " + AppKey); 
//
//
//connected = modem.joinOTAA(AppEUI, AppKey, DevEUI); 
//
//
//if (connected) 
//{
//  Serial1.println("Joined");  //Connection established - PB5 turns on
//  pinMode(PB5, OUTPUT);
//} 
//else 
//{
//  Serial1.println("Join failed"); 
//  while (true) {};
//}
  
}

void send_packet() 
{
  char payload[27] = { 0 }; 
  sprintf(payload,"Hello World !");  //Format data into payload buffer

  modem.setPort(3);  //Set TX port
  modem.beginPacket(); 
  modem.write(payload, strlen(payload));

  if (modem.endPacket() == (int)strlen(payload)) 
  {
    Serial1.println("Sent packet");
  } 
  else 
  {
    Serial1.println("Failed to send packet");
  }
}

void loop() 
{
  if (!last_tx || millis() - last_tx > TX_INTERVAL) 
  {
    send_packet();
    last_tx = millis();
  }
}
