# esp8266C64

This is a C64 Emulator running on an esp8266 using a webbrowser as a display.

For more info see http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932.
The code was based upon http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065

Remarks regarding speed:
The max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
Reduced the number of RAM bytes send from esp8266 to zero and
also reduced the processing of this info in browser to zero bytes,
but this has no effect on framerate (still 5 fps)
slow speed possibly due to protocolo XMLHTTP!
Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

