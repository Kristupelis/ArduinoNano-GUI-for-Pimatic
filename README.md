# ArduinoNano-GUI-for-Pimatic
This project was made to show of how ArduinoNano module behaves when it's connected to Pimatic

--------------------------------------------------------------------------------------------

To understand the project and its needs, we have to understand how everything works. First of all, we have a Linux virtual machine, which has a .c code that simulates Homeduino protocol commands such as "DW 13 1", "AR 7" and similar on ArduinoNano module. These commands are being sent to Pimatic service via serial port pairs /dev/tnt0 <> /dev/tnt1. How to set up such serial ports you can read: https://github.com/freemed/tty0tty. Then in pimatic we can see the simulated temperature, humidity and other incoming commands. However, we are not only receiving, but also sending commands from Pimatic to the .c code when for example we flick a switch in Pimatic GUI. That's how we communicate with serial devices.

The problem is that we would have to have a physical ArduinoNano module, DHT senor, buttons and other components and connect them to our computer to see, how these componets behave when we press buttons, turn on switches or do something else. To solve this problem, I made a Virtual ArduinoNano module, DHT sensor and whatever else you want to have, since the code can be freely adjusted by your needs. For example you can even have 1000 pins if that's your desire:).

This solution works with ser2net technology (more information about it in: https://github.com/cminyard/ser2net. What I did first was I created a GUI for ArduinoNano and other components using Processing4.3 (Java programming language). It's a very useful tool if you want to create something visually working. This Processing4.3 code also connects to my personal computer virtual port COM1 which was created using VSPE. The only thing left was to update my .c code to not only send and receive data from Pimatic, but also send and receive data from my personal computers Processing4.3 code. For this connection I created a ser2net service (TCP network bridge), changed .yaml and .conf files and that's it. I just had to connect my new /dev/tnt pair with my virtual port on my personal computer via network.

[image](https://github.com/user-attachments/assets/9ff1a95b-86f1-493a-a29a-7d9397fb936d)
