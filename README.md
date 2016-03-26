# esp8266C64

C64 Emulator, works with esp8266 using webbrowser as display
see http://www.esp8266.com/viewtopic.php?f=8&t=4704&p=28932#p28932
based upon http://forum.arduino.cc/index.php?topic=193216.msg1793065#msg1793065

Remarks speed:
max speed is 5 frames per second, this amounts to 5 kB (kilobytes) per second
Reduced the number of RAM bytes send from esp8266 to zero and
also reduced the processing of this info in browser to zero bytes,
but this has no effect on framerate (still 5 fps)
slow speed possibly due to protocolo XMLHTTP!
Serial debug works at 115200 baudrate, so 14kB per second and would make 14 fps possible!

