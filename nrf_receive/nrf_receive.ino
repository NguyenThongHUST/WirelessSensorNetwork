
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(12,5); // CE, CSN
uint8_t RxPipeAddrs[5] = {0x11 , 0x22, 0x33, 0x44, 0xAA} ;

struct sensor_data
{
  int sensor_id ;
  int sensor_value ;
} ;
struct sensor_data fire_sensor ;

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setAutoAck(false) ; // Receivers send ACK to transmitter
  radio.setChannel(52) ;
  radio.setPayloadSize(32) ;
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(0, RxPipeAddrs);
  radio.startListening();
}

void loop() {

   if (radio.available())
  {
    radio.read(&fire_sensor, sizeof(fire_sensor));
    delay(10);
    Serial.println(fire_sensor.sensor_id);
    Serial.println(fire_sensor.sensor_value);
  }
}
