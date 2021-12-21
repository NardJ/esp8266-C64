const char * redir = "<!doctype html><html lang=hu><meta http-equiv=\"refresh\" content=\"0;url=http://10.1.1.1/\">";

#if !WIFI_MODE
// CaptivePortal from DNS-server example
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(10, 1, 1, 1);
DNSServer dnsServer;
#endif

void APsetup() {
#if !WIFI_MODE

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);
#else

  WiFi.forceSleepWake();
  delay(10);

  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(10);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);
  bool wfBlink = false;

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }
#endif

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.onNotFound([]() {
    server.send(200, "text/html", redir );
  });

  server.on("/default", []() {
    server.send(200, "text/html", redir);
  });
  //
  //  server.on("/gui", []() {
  //    server.send(200, "text/html", FPSTR(gui) );
  //  });
  //
  //  server.on("/VICII.js", []() {
  //    server.send(200, "application/javascript", FPSTR(vicii) );
  //  });
}

void APloop() {
#if !WIFI_MODE
  dnsServer.processNextRequest();
#endif
}

void SerialNotFound() {
  String message = "";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  Serial.println(message);
}
