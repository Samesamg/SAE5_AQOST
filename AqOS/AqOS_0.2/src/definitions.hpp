#ifndef __DEFINITIONS_HPP__
#define __DEFINITIONS_HPP__

//PINS DEFINITIONS
//UART
#define UART_RX_PIN PB7
#define UART_TX_PIN PB6

//GPIOs
#define MAINTENANCE_PIN PC0
#define EEPROM_PIN PC1
#define BAT_ADC_PIN PA10
#define LED_PIN PB15
#define TEMP_SENSOR_PIN PB5


//TIMINGS 
#define SEND_TIMEOUT 10  // 900s = 15min
#define WAKEUP_TIMEOUT 5000  //ms

#define COUNTER_RESET 96  //Counter incrementing at each temp send => resets each 24h

#endif 