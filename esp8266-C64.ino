/* 
  C64 Emulator, works with esp8266 using webbrowser as display
  see
  http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932
  based upon
  http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065
  
  Remarks speed:
  max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
  Reduced the number of RAM bytes send from esp8266 to zero and
  als reduced the processing of this info in browser to zero bytes,
  but this has no effect on framerate (still 5 fps)
  slow speed possibly due to protocolo XMLHTTP!
  Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

  ### Android phones are incompatible due missing keyboard...
  
  ### Code is now prepared for
  - Arduino IDE 1.8.10
  - esp8266 board tools version 2.4.2 (important!) 
  - LwIP 1.4 Higher bandwidth
  - CPU frequency 160MHz
  
  This sketch will create free WiFi-AP named Commodore 64 - tap on it and enjoy
*/
//find free space 
//extern "C" {#include "user_interface.h"}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "webpages.h"

//const char* ssid = "DeltaFlyer";
const char* ssid = "Voyager";
const char* password = "keelta01";

ESP8266WebServer server(80);
uint8_t curkey = 0;

#define EXECUTES_PER_RUN 100
#define RAM_SIZE 24576 
uint8_t RAM[RAM_SIZE];
uint8_t HIGHVIC[16];//color of background/border/sprites are situated here

uint16_t getpc();
uint8_t getop();
void exec6502(int32_t tickcount);
void reset6502();
void serout(uint8_t val) {
		Serial.write(val);
}


void setup () {
  delay(1000);
  Serial.begin (115200);
  Serial.println("Setup Start");
}

unsigned long cycles=0;
void loop () {
  init_fase();  
  server.handleClient();
  APloop();
  
  exec6502(EXECUTES_PER_RUN);    
  cycles++;
}
