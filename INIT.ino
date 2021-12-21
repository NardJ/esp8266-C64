
void initWifi() {
  APsetup();
}


void initServer() {
  // set comm. pages
  server.on ( "/", []() {
    Serial.println("Root page called");
    //server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send_P ( 200, "text/html", gui);
  });
  server.on ( "/VICII.js", []() {
    Serial.println("Javascript loaded");
    //server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send_P ( 200, "text/plain", vicii);
  });
  //server.on ( "/VICIIRefresh", VICIIRefresh);

  //  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }



  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);

}

/*
  void checkFreeSpace(){
  //uint32_t freeSpace=system_get_free_heap_size();
  Serial.print (system_get_free_heap_size());
  Serial.println (" free space.");
  }
*/

int fase = 0;
void init_fase() {
  if (fase != 4) {
    Serial.print ("Start boot fase ");
    Serial.println(fase);
  }
  if (fase == 0) {
    initWifi();
  }
  if (fase == 1) {
    initServer();
  }
  //if (fase==2) {checkFreeSpace();}
  if (fase == 3) {
    reset6502();
  }
  if (fase != 4) {
    Serial.print ("..finished boot fase ");
    Serial.println(fase);
  }
  if (fase != 4) {
    fase++;
    return;
  }
}
