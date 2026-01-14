#include <Arduino.h>
#include "STM32LoRaWAN.h"
#include "DS18B20.h"

STM32LoRaWAN modem;
HardwareSerial Serial1(PB7, PB6); //(RX, TX)
String msg;


OneWire oneWire(PB5);
DS18B20 sensor(&oneWire);

static const unsigned long TX_INTERVAL = 60000; /* ms */
unsigned long last_tx = 0;



void setup() {
  pinMode(PC0, INPUT);
  pinMode(PB10, OUTPUT);
  pinMode(PA9, OUTPUT);
  digitalWrite(PB10, HIGH); //enable 5V
  digitalWrite(PA9, HIGH);  //enable 3V3
  Serial1.begin(115200);
  Serial1.println("Start");
//  modem.begin(EU868);

//  bool connected = modem.joinOTAA(/* AppEui */ "000000000000EA29", /* AppKey */ "509E1667257F5594DFBC03AB67A89B43", /* DevEui */ "70B3D57ED00752BF");

/*  if (connected) 
  {
    Serial1.println("Joined");
  } 
  else 
  {
    Serial1.println("Join failed");
    while (true) 
    {
    }
  }
*/
  pinMode(PB5, OUTPUT);  
  sensor.setResolution(12);
}

void send_packet(String message) {
  const char * msg = message.c_str();
  Serial1.println(message);
  char payload[27] = { 0 }; 
  sprintf(payload,msg);

  modem.setPort(3);
  modem.beginPacket();
  modem.write(payload, strlen(payload));
  if (modem.endPacket() == (int)strlen(payload)) {
    Serial1.println("Sent packet");
  } else {
    Serial1.println("Failed to send packet");
  }

  if (modem.available()) {
    Serial1.print("Received packet on port ");
    Serial1.print(modem.getDownlinkPort());
    Serial1.print(":");
    while (modem.available()) {
      uint8_t b = modem.read();
      Serial1.print(" ");
      Serial1.print(b >> 4, HEX);
      Serial1.print(b & 0xF, HEX);
    }
    Serial1.println();
  }
}

void loop() {
  //Serial1.println("entering loop");
  if (digitalRead(PC0) == 0) 
  {
  sensor.requestTemperatures();
  String msg = String(sensor.getTempC());
//  send_packet(msg);
  Serial1.println(msg);
  
  last_tx = millis();
  }
}
