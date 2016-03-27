##ESP8266-C64

This is a C64 Emulator running on an esp8266 using a webbrowser as a display.
For more info see [http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932](http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932).
The code was based upon [http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065](http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065)

###Installation
1. Extract the files in the zip
2. Start Arduino editor and open the ino file
2. Upload the ino file to the esp8266
3. Browse to the ip address of your esp8266

###Have fun
You should now see the startup screen of de C64!

![alt text](https://upload.wikimedia.org/wikipedia/commons/4/48/C64_startup_animiert.gif "C64 startup screen")

Using pokes you can change the background color, position of text and write character to screen:
```basic
  POKE 53280,2
  POKE 53281,5
```  
Of course you can also enter simple programs like: 
```basic
  10 PRINT "C64 lives!"
  20 GOTO 10
```

###Remarks regarding speed:
The emulation speed within the esp8266 is higher than the update speed of the browser. The maximum speed of the browser implementation is 5 frames per second, possibly due to the use of the XMLHTTP protocol! Any ideas/code suggestions how to solve this are most welcome!

