##esp8266-C64 <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/5/5c/CBM_Logo.svg/150px-CBM_Logo.svg.png" width="48" align="top" style="float: right;">


This is a C64 Emulator running on an esp8266 using a webbrowser as a display.
For more info see [http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932](http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932).
The code was based upon [http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065](http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065)

###Installation
1. Extract the files in the zip
2. Start Arduino editor and open the ino file
2. Upload the ino file to the esp8266
3. Browse to the ip address of your esp8266

###Have fun
You should now see the startup screen of de C64 (possibly in other colors)!

![alt text](https://upload.wikimedia.org/wikipedia/commons/4/48/C64_startup_animiert.gif "C64 startup screen")

Using pokes you can change the background color, position of text and write character to screen.
Of course you can also enter simple programs like 
```basic
  10 print "C64 lives!"
  20 goto 10
```

###Remarks regarding speed:
The max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
Reduced the number of RAM bytes send from esp8266 to zero and
also reduced the processing of this info in browser to zero bytes,
but this has no effect on framerate (still 5 fps)
slow speed possibly due to protocolo XMLHTTP!
Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

