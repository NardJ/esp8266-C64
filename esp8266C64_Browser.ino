// C64 Emulator, works with esp8266 using webbrowser as display
// see
// http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932
// based upon
// http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065

// Remarks speed:
// max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
// Reduced the number of RAM bytes send from esp8266 to zero and
// als reduced the processing of this info in browser to zero bytes,
// but this has no effect on framerate (still 5 fps)
// slow speed possibly due to protocolo XMLHTTP!
// Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

//find free space 
//extern "C" {#include "user_interface.h"}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

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

//server.send_uint8(200, "text/plain", msg);   
void send_uint8(const uint8_t *data, const size_t data_length) {
/*
 else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  //if(server.hasArg("download")) dataType = "application/octet-stream";
*/  
  String data_type="application/octet-stream";
  //String data_type="image/png";
  server.sendHeader("Content-Length", String(data_length));
  server.send(200, data_type.c_str(), "");

  WiFiClient client = server.client();
  client.write(data, data_length);
}

const char gui[] PROGMEM = 
  "<html><head>\n"
  "<meta name='viewport' content='width=device-width, initial-scale=1' />\n"
  "<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />\n"
  "<meta http-equiv='Pragma' content='no-cache' />\n"
  "<meta http-equiv='Expires' content='0' />\n"
  "<script src='VICII.js'></script>\n"
  "</head>\n"
  "<body>\n<h1 style='font-family:Lucida Console;font-size:32px;margin:0px;padding:0px'>VIC-II client</h1>\n"
  "<div style='width:384px;font-size:12px;'><div style='float:right;'>for use with ESP8266 based C64 CPU emulator</div><br><div style='float:right;'><a href='http://www.esp8266.com/viewtopic.php?f=32&t=4596&start=5'>info at esp8266.com forum</a></div></div>\n"
  "<div id='fps' style='background-color:#FFFFAA;border:1px solid black;width:42px;height:24px;text-align:center;line-height:22px;font-size:12px;'>10 fps</div>"
  "<canvas id='vicoutput' style='background-color:#A4A4FE;border:1px;width:384px;height:264px;'></canvas>\n"
  "<DIV id='Log' style='background-color:#FFFFAA;width:378px;height:80px;border:1px solid black;font-family:Lucida console;font-size:10px;padding:2px;overflow:auto;'></div>\n"
  "<DIV id='PopUp'\n"
  " style='display: none; position: absolute; left: 30px; top: 50px; border: solid black 1px; padding: 10px; background-color: rgb(240,240,240); text-align: justify; font-size: 12px; width: 360px;'\n"
  " onclick='document.getElementById('PopUp').style.display = 'none''>\n"
  "<SPAN id='PopUpText' style='font-family:Lucida Console;letter-spacing: 2px;'>TEXT</SPAN>\n"
  "</DIV>\n"
  "</body></html>\n";

//Onready vindt alleen plaats als er iets wordt verzonden naar de browser!
//responeType arrayBuffer werkt niet goed!!! ALternatief is blob?    
//    "  xmlhttp.responseType = 'arraybuffer';\n"

const char vicii[] PROGMEM =   
    "var canvas;\n" 
    "var ctx;\n" 
    "var VICRAM;\n" 
    "var RAM_SIZE=1025;\n"
    "var interval;\n"
    "var xmlhttp=new XMLHttpRequest();\n"    
    "function printLog(msg){var log=document.getElementById('Log');log.innerHTML=log.innerHTML+msg+'<br>';log.scrollTop = log.scrollHeight}\n"    
    "document.addEventListener('DOMContentLoaded',initCanvas,false);\n"
    "var fC=0;\n"
    "var cS=0;\n"
    "setInterval(updateFPS,1000);\n"
    "setInterval(function(){cS=!cS;},500);\n"
    "function updateFPS(){\n"
    "  var fT = document.getElementById('fps');\n"
    "  fT.innerHTML=fC+ ' fps';\n"
    "  fC=0;"
    "}\n"    
    "function initCanvas(){\n"    
    "  printLog('initCanvas');\n"
    "  canvas = document.getElementById('vicoutput');\n"
    "  canvas.width=384;\n"
    "  canvas.height=264;\n"
    "  ctx = canvas.getContext('2d');\n"
    "  ctx.font = '12px Lucida Console';\n"    
    "  VICRAM = new Uint8Array(RAM_SIZE);\n"
    "  refreshVICRAM();\n"
    "  document.addEventListener('keypress', keypress, false);\n"
    "}\n"
    "var isAlive=true;\n"
    "var aliveTimer;\n"
    "function alive( ){\n"
    "  if (!isAlive) {\n"
    "    xmlhttp.abort();refreshVICRAM();\n"
    "    printLog('Restarting connection');\n"
    "  }\n " 
    "}\n"
    "function refreshVICRAM( ){\n"
    // based on https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Sending_and_Receiving_Binary_Data
    "  xmlhttp.onreadystatechange=function(){\n"
    "    if (xmlhttp.readyState==4 && xmlhttp.status==200){\n"
    "      isAlive=true;\n"
    "      var aB=xmlhttp.response;\n"
    "      if (aB){\n"
    "        VICRAM = new Uint8Array(aB);\n"
    "        refreshDisplay();\n"
    "        setTimeout(refreshVICRAM,0);\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "  var tp='GET';\n"
    "  var url='/VICIIRefresh';\n"    
    "  if (keyp.length>0) {tp='POST';url=url+keyp;}\n"
    "  keyp='';\n"
    "  xmlhttp.open(tp,url,true);\n"
    "  xmlhttp.setRequestHeader('Content-type','application/octet-stream');\n"
    "  xmlhttp.responseType='arraybuffer';\n"
    "  xmlhttp.send();\n"
    "  isAlive=false;\n"
    "  clearTimeout(aliveTimer);\n"
    "  aliveTimer=setTimeout(alive,3000);\n"    
    "}\n"
/*    
    "function dumpVICRAM(){\n"
    "  var msg='';\n" 
    "  for (var i=0; i<120; i++){\n"
    "    var petscii = VICRAM[i];\n"
    "    if (petscii<32) petscii = petscii + 64;\n"    
    "    msg=msg+String.fromCharCode(petscii);\n"
    "    var col=i % 40;\n"
    "    if (col==39) msg=msg+'<BR>';\n"
    "  }\n"
    "  printLog(msg);\n"
    "}\n"
*/    
  //https://www.c64-wiki.com/index.php/Color
    "var pal=['#000000','#FFFFFF','#880000','#AAFFEE','#CC44CC','#00CC55','#0000AA','#EEEE77','#DD8855','#664400','#FF7777','#333333','#777777','#AAFF66','#0088FF','#BBBBBB'];\n"
    "function refreshDisplay(){\n"
//    "  printLog('refreshDisplay');\n"
    "  var tcol=pal[VICRAM[4]];\n"//text color
    "  var fcol=pal[VICRAM[5]];\n"//frame/border color
    "  var bcol=pal[VICRAM[6]];\n"//back color
    "  ctx.fillStyle=fcol;\n"
    "  ctx.fillRect(0,0,384,264);\n"
    "  ctx.fillStyle=bcol;\n"
    "  ctx.fillRect(32,32,320,200);\n"
    "  ctx.font='10px Lucida console';\n"
    "  ctx.textBaseline='top';\n"
    "  ctx.textAlign='left';\n"
    "  ctx.fillStyle=tcol;\n"
    "  var petscii;\n"
    "  for (var col=0;col<40;col++){\n"
    "    for (var row=0;row<40;row++){\n"
    "      petscii=VICRAM[24+row*40+col];\n"//char rom starts at 24 offset 
    "      if (petscii<32) petscii = petscii + 64;\n"
    "      ctx.fillText(String.fromCharCode(petscii),32+col*8,32+row*8);\n"
    "    }\n"
    "  }\n"
    "  if (cS) ctx.fillRect(32+VICRAM[1]*8,32+VICRAM[0]*8,8,8);\n"
    "  fC++;\n"  
    "}\n"
    "var keyp='';\n"
    "function keypress(e) {\n"
    "  var keyCode = e.keyCode;\n"
    "  if (keyp.length==0) {keyp=keyp+'?';}else{keyp=keyp+'&';}\n"
    "  keyp=keyp+'k='+keyCode.toString();\n"
//    "  printLog('keyDown: '+keyCode);\n"
    "}\n"
    ;

/*
void handleNotFound(){
  String message = "File Not Found: ";
  server.send(404, "text/plain", message);
  message += "'"+server.uri()+"'";
  Serial.println(message);
}
*/

void dumpScreenRam(){
  //Serial.println ("Cycles:"+cycles);
  for (int i=0; i<1000; i++){
    uint8_t petscii = RAM[i + 1024];
    if (petscii<32) petscii = petscii + 64;
    Serial.print (char(petscii));
    int col=i % 40;
    if (col==39) Serial.println();
  }
}

void VICIIRefresh(){
  wdt_disable();

  //handle keypresses
  if (server.args()){
     //Serial.println("keyDown: ");
     //code using http://commodore64.se/wiki/index.php/Commodore_64_RAM_Addresses    
     int nrkeys=0;  
     for (uint8_t i=0; i<server.args(); i++){ 
       if (nrkeys<10 && server.argName(i).equals("k")){
         int key=server.arg(i).toInt();         
         //if (key>63 && key<94) key = key - 64;
         //make all upper case
         if (key>96 && key<123) key = key - 32;
         key = key & 0x7F;
         RAM[631+nrkeys] = key ;//& 0x7F;//put key in buffer
         nrkeys++;         
/*         
         Serial.print("accept: ");
         Serial.print(server.arg(i).toInt());
         Serial.print(" > ");
         Serial.println(key);
       }else{
         //Serial.print("reject: ");
         //Serial.print(server.argName(i));              
         //Serial.print("=");
         //Serial.println(server.arg(i));
*/         
       }
     }      
     RAM[198]=nrkeys;//number keys put in buffer
  }

//245-246 vector keyboard decode table
//655-656 vector keyboard table setup
//648 top of screen memory
//678 PAL(1)/NTSC(0)
//2024-?2047? sprite memory

  //send video ram

  int m=0;
  uint8_t VICRAM[1024];  
  VICRAM[0]=RAM[214];//line number of cursor (read/write)
  VICRAM[1]=RAM[211];//col number of cursor (read/write)
  VICRAM[2]=RAM[243];//low byte of location of color ram (RAM[244]*256+RAM[243], 55296-56319 default )
  VICRAM[3]=RAM[244];//high byte of location of color ram (RAM[244]*256+RAM[243], 55296-56319 default )
  VICRAM[4]=RAM[646];//text color code (read/write)
  VICRAM[5]=HIGHVIC[0];//53280 border color
  VICRAM[6]=HIGHVIC[1];//53281 background (0)
  VICRAM[7]=HIGHVIC[2];//53282 background 1 (MC & ECM)
  VICRAM[8]=HIGHVIC[3];//53283 background 2 (MC & ECM)
  VICRAM[9]=HIGHVIC[4];//53284 background 3 (MC & ECM)
  m=24;
  for (int i=1024;i<2023;i++){//1024-2023 char ram
    uint8_t petscii = RAM[i];
    if (petscii<32) petscii = petscii + 64;
    VICRAM[m]=petscii;
    m++;
  }
  send_uint8(VICRAM,1024); 
  //dumpScreenRam();
  wdt_enable(5000);  
} 


//Very nice articles about graphics mode of the VIC-II
//memory requirements of different modes: http://www.filfre.net/2013/08/how-things-work-commodore-64-and-summer-games-edition/
//location of screen RAM @ 53272 (https://www.c64-wiki.com/index.php/Screen_RAM)
//how it works: http://dustlayer.com/c64-coding-tutorials/2013/4/8/episode-2-2-writing-to-the-c64-screen

void setup () {
  delay(1000);
  Serial.begin (115200);
  Serial.println("Setup Start");
}

unsigned long cycles=0;

void initWifi(){
  // Start Wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("  ip-address: ");
  Serial.println(WiFi.localIP());
}

void initServer(){
  // set comm. pages    
  server.on ( "/",[]() {
                    Serial.println("Root page called"); 
                    server.send_P ( 200, "text/html", gui); 
                   });
  server.on ( "/VICII.js",[]() {
                    Serial.println("Javascript loaded"); 
                    server.send_P ( 200, "text/plain", vicii); 
                   });             
  server.on ( "/VICIIRefresh", VICIIRefresh);

//  server.onNotFound(handleNotFound);
  
  // Start the server  
  server.begin();
}

/*
void checkFreeSpace(){
  //uint32_t freeSpace=system_get_free_heap_size();
  Serial.print (system_get_free_heap_size());
  Serial.println (" free space.");
}
*/

int fase=0;
void loop () {
  if (fase!=4) {Serial.print ("Start boot fase ");Serial.println(fase);}
  if (fase==0) {initWifi();}
  if (fase==1) {initServer();}
  //if (fase==2) {checkFreeSpace();}
  if (fase==3) {reset6502();}
  if (fase!=4) {Serial.print ("..finished boot fase ");Serial.println(fase);}
  if (fase!=4) {fase++;return;}
  
  server.handleClient();
  
  exec6502(EXECUTES_PER_RUN);    
  cycles++;
}

