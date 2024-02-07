## ESP8266-C64

This is a C64 Emulator running on an esp8266 using a webbrowser as a display.
For more info see [http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932](http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932).
The code was based upon [http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065](http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065)

### Android phones are incompatible due missing keyboard

### Code is now prepared for
- Arduino IDE 1.8.10
- esp8266 board tools version 2.4.2 (important!) 
- LwIP 1.4 Higher bandwidth
- CPU frequency 160MHz

### Installation
1. Extract the files in the zip
2. Run generate.sh
3. Start Arduino editor and open the ino file
4. Setup above settings in editor
5. Set WIFI_MODE in line 37 to use host/client mode 
6. Create secret.sh file with two lines: #define STASSID "Your SSID" #define STAPSK "Your password"
7. Upload the ino file to the esp8266
8. This sketch will create a free WiFi-AP named STASSID or connect to your wifi router, the IP will be on the serial monitor
10. Enjoy...

### Have fun
You should now see the startup screen of de C64!

![alt text](https://upload.wikimedia.org/wikipedia/commons/4/48/C64_startup_animiert.gif "C64 startup screen")

### BASIC example
```basic
  10 FOR I = 1 TO 10
  20 PRINT "HELLO WORLD"
  30 NEXT I
  RUN
```
Using pokes you can change the background color, position of text and write character to screen:
```basic
  POKE 53280,2
  POKE 53281,5
```  
Of course you can also enter simple programs like: 
```basic
  10 PRINT "C64 lives!"
  20 GOTO 10
  RUN
```

### Tetris BASIC code example
Clipboard does work, use CTRL+V (dont paste too much text)...
```basic
1 a$="efijefijefijefijbfjnhijkbfjnhijkijfgaefjijfgaefjefjkiefbefjkiefbbfjidefj" 
2 a$=a$+"abeieijkaeijijkgabfjiefgehijebfj@abe@dhe":o=207:dime(o):forx=0to111 
3 print,"   {CBM-M}","    {CBM-G}":p=asc(mid$(a$,x+1)):e(x)=(pand3)+(pand12)*10:next:m=2024 
4 print,"    {CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}{CBM-T}":gosub6:goto7 
5 pokei+e(r),c:pokei+e(r+1),c:pokei+e(r+2),c:pokei+e(r+3),c:c=160:r$="d":return 
6 i=1152:r=h:c=32:gosub5:j=int(rnd(0)*7)*16:r=j:gosub5:r=h:h=j:i=i+9:return 
7 gosub6:w=i:t=i:g=r:k=240:l=1278 
8 gosub15:c=32:gosub5:r=-r*b-g*notb:g=r 
9 i=w:w=i+40:gosub5:gosub15:ifbthen12 
10 getk$:g=randkor((r-4*(k$="s"))and15) 
11 t=w:w=w+(k$=l$)-(k$=r$):l=l+40:goto8 
12 c=o:gosub5:m=m-(l<m)*(l-m):fory=0to3:q=1:forx=0to9:q=q*(peek(l+x)=o):next 
13 fors=ltol+(m-160-l)*qstep-1:pokes+11,peek(s-29):next:y=y-q:m=m+q*40:z=z+q 
14 l=l+40*not-q:next:print"{home}";z:goto7 
15 b=0:forx=0to3:b=bor1andpeek(w+e(g+x)):next:b=b>0:w=-t*b-w*notb:l$="a":return 
```
Note that the {CBM-x} represents the graphic on that particular key (press the c= key and the letter to produce it)

For more examples follow links at: 
https://github.com/matgoebl/esp8266-ntsc-c64-emulator/tree/master/prg
https://gkanold.wixsite.com/homeputerium/kopie-von-games-list-2019-2

### Remarks regarding speed:
The emulation speed within the esp8266 is close to the update speed of the browser.
