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
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include "secret.h"

#define WIFI_MODE 1 //0 => AP (host) | 1 => STA (client)

const char* ssid = STASSID;
const char* password = STAPSK;

WebSocketsServer webSocket = WebSocketsServer(81);
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
//  Serial.println("Setup Start");
}

unsigned long last_10sec = 0;
unsigned int counter = 0;

unsigned long cycles = 0;

byte WSC = 255;
byte overHead = 10;
byte overHeadC = 0;

void loop () {
  init_fase();
  unsigned long t = millis();
  webSocket.loop();
  server.handleClient();
//  Serial.println("in loop");

  if (WSC != 255 && (overHeadC % overHead) == 0) {
    VICIIRefreshWS(WSC);
    
    //Serial.print("sending to ");
    //Serial.println(WSC);
  }
  overHeadC+=1;
  
  if ((t - last_10sec) > 10 * 1000) {
    counter++;
    bool ping = (counter % 2);
    int i = webSocket.connectedClients(ping);
    Serial.printf("%d Connected websocket clients ping: %d\n", i, ping);
    last_10sec = millis();
  }

  APloop();

  exec6502(EXECUTES_PER_RUN);
  cycles++;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      WSC = 255;
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
//        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);


        WSC = num;
        // send message to client
        //webSocket.sendTXT(num, "Connected");
        VICIIRefreshWS(num);
      }
      break;
    case WStype_TEXT:
//      Serial.printf("[%u] get Text: %s\n", num, payload);

      GetChar(payload, length);

      //      if (payload[0] == '#') {
      //        // we get RGB data
      //
      //
      //      }

      break;
  }

}
