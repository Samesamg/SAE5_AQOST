/*
AQOST - AqOS_0.1 - 20/11/2025
Basic LoRa TX with Seeed E5 module - Based on STM32duinoLoRaWAN "LoRa send and receive" example
*/

#include <Arduino.h>
#include "STM32LoRaWAN.h"

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

  Serial1.begin(115200);
  Serial1.println("Start");

  modem.begin(EU868);  //init SubGhz
  connected = modem.joinOTAA("000000000000E5DD", "F04FB86BBD54BBE33392F77CBE59F806", "70B3D57ED0072EE0");  // (AppEui, Appkey, DevEui)  TODO : get device EUI from unique identifier

  if (connected) 
  {
    Serial1.println("Joined");  //Connection established - PB5 turns on
    pinMode(PB5, OUTPUT);
  } 
  else 
  {
    Serial1.println("Join failed"); 
    while (true) {};
  }
  
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
