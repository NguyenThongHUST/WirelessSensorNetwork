
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
            
#include <Wire.h>
#include "SparkFunHTU21D.h"
            
#define RXD2 16
#define TXD2 17

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdio.h>
#include <string.h>

RF24 radio(12,5); // CE, CSN

//address (40 bit) through which two modules communicate.
uint8_t RxPipeAddrs[5] = {0x11 , 0x22, 0x33, 0x44, 0xAA} ;

byte counter = 1; //used to count the packets sent
unsigned long timer1 = 0 , timer2 = 0 ;


struct sensor_data
{
  int sensor_id ;
  int sensor_value ;
} ;

struct sensor_data fire_sensor_data ;
struct sensor_data gas_sensor_data ;

float sensor_gas_val = 0;
      
void fire_detection(void *parameter){
   while(1){
      int sensorReading = analogRead(GPIO_NUM_34);
      fire_sensor_data.sensor_id = 1 ;
      fire_sensor_data.sensor_value = sensorReading ;
      Serial.println(sensorReading);
    
      if(sensorReading <= 500) 
      {
        Serial.println("|Fire");
      }
      
      else if (sensorReading >=800 )
      {
        Serial.println("|No Fire");
      }
      delay(50);
            
      timer1 = micros() ;
      if( radio.write(&fire_sensor_data, sizeof(fire_sensor_data)) ) {
         Serial.println("Successfull"); 
         counter = counter + 1 ;
         timer2 = micros() ;
         Serial.println(counter);
         Serial.println((timer2 - timer1));
       }
       Serial.println();
       vTaskDelay(2000 / portTICK_PERIOD_MS);  // delay between reads
   }  
}

void gas_detection(void *parameter){
   while(1){   
     sensor_gas_val = analogRead(GPIO_NUM_4);
     Serial.println(sensor_gas_val);

      gas_sensor_data.sensor_id = 2 ;
      gas_sensor_data.sensor_value = sensor_gas_val ;
      
     if(sensor_gas_val > 550)
      {
        Serial.println(" | Smoke detected!");
      }
    else if (sensor_gas_val < 480) 
      {
        Serial.println(" |No Smoke detected!");
      }
    delay(50);

      if( radio.write(&gas_sensor_data, sizeof(gas_sensor_data)) ) {
         Serial.println("Successfull"); 
         counter = counter + 1 ;
         Serial.println(counter);
         Serial.println((timer2 - timer1));
       }
       Serial.println();
    
    vTaskDelay(2000 / portTICK_PERIOD_MS); 
  }  
}
      

void setup() {
  
  Serial.begin(115200);
  
  radio.begin();
  radio.setAutoAck(false) ; // Receivers send ACK to transmitter
  radio.setRetries(5,15); // 15 retries, 5ms
  radio.setChannel(52) ;
  radio.setPayloadSize(32) ;
  radio.setDataRate(RF24_2MBPS);
  
  //set the address
  radio.openWritingPipe(RxPipeAddrs);
  radio.stopListening(); // transmit mode
  radio.printDetails();

  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
                    fire_detection,  // Function to be called
                    "frame sensor",   // Name of task
                    1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                    NULL,         // Parameter to pass to function
                    3,            // Task priority (0 to configMAX_PRIORITIES - 1)
                    NULL,         // Task handle
                    app_cpu);     // Run on one core for demo purposes (ESP32 only) 

   xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
                    gas_detection,  // Function to be called
                    "gas detection",   // Name of task
                    1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
                    NULL,         // Parameter to pass to function
                    2,            // Task priority (0 to configMAX_PRIORITIES - 1)
                    NULL,         // Task handle
                    app_cpu);     // Run on one core for demo purposes (ESP32 only)     
   vTaskDelete(NULL);
}

void loop() {
   
}
