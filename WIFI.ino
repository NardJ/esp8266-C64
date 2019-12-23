// CaptivePortal from DNS-server example
#include <DNSServer.h>

const byte DNS_PORT = 53;
//IPAddress apIP(172, 217, 28, 1);
IPAddress apIP(10, 1, 1, 1);
DNSServer dnsServer;

void APsetup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Commodore C64");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

// replay to all requests with same HTML
  server.onNotFound([]() {
    Serial.println("Captive page..."); 
    //server.send(200, "text/html", "<h1><br><br><a href=http://10.1.1.1/>Commodore C-64 BASIC emulator</a><br><br></h1>");
    server.send_P ( 200, "text/html", gui); 
    //SerialNotFound(); // uncomment for request diagnostics
  });
}

void APloop() {
  dnsServer.processNextRequest();
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
