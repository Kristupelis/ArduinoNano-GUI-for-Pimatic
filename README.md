# ArduinoNano-GUI-for-Pimatic
This project was made to show off how ArduinoNano module behaves when it's connected to Pimatic

--------------------------------------------------------------------------------------------

To understand the project and its needs, we have to understand how everything works. First of all, we have a Linux virtual machine, which has a .c code that simulates Homeduino protocol commands such as "DW 13 1", "AR 7" and similar on ArduinoNano module. These commands are being sent to Pimatic service via serial port pairs /dev/tnt0 <> /dev/tnt1. How to set up such serial ports you can read: https://github.com/freemed/tty0tty. Then in pimatic we can see the simulated temperature, humidity and other incoming commands. However, we are not only receiving, but also sending commands from Pimatic to the .c code when for example we flick a switch in Pimatic GUI. That's how we communicate with serial devices.

The problem is that we would have to have a physical ArduinoNano module, DHT senor, buttons and other components and connect them to our computer to see, how these componets behave when we press buttons, turn on switches or do something else. To solve this problem, I made a Virtual ArduinoNano module, DHT sensor and whatever else you want to have, since the code can be freely adjusted by your needs. For example you can even have 1000 pins if that's your desire:).

This solution works with ser2net technology (more information about it in: https://github.com/cminyard/ser2net). What I did first was I created a GUI for ArduinoNano and other components using Processing4.3 (Java programming language). It's a very useful tool if you want to create something visually working. This Processing4.3 code also connects to my personal computer virtual port COM1 which was created using VSPE. The only thing left was to update my .c code to not only send and receive data from Pimatic, but also send and receive data from my personal computers Processing4.3 code. For this connection I created a ser2net service (TCP network bridge), changed .yaml and .conf files and that's it. I just had to connect my new /dev/tnt pair with my virtual port on my personal computer via network.

--------------------------------------------------------------------------------------------

Steps to run this solution:
1. Install ser2net

		sudo apt install ser2net -y

2. Verify installation:

		ser2net --version
   
3. Update your ser2net.conf and ser2net.yaml files in /etc folder with (or just download them from this repository and replace the old ones):

     	sudo nano /etc/ser2net.conf
   		sudo nano /etc/ser2net.yaml

5. Enalbe/restart/stop or check status for ser2net service (don't forget to restart ser2net if you changed .yaml or .conf files):

		sudo systemctl enable ser2net
		sudo systemctl restart ser2net
		sudo systemctl start ser2net
		sudo systemctl status ser2net

6. Copy VhduinoU.c file from your Windows computer to your virtual machine with:

		scp C:\Users\[YourWindowsUsername]\Desktop\Patobulinta\VhduinoU.c root@158.129.30.xxx:/root

7. Compile this C code on your virtual machine with:

		gcc -o VhduinoU VhduinoU.c

8. Open VSPE and load the configuration file (ser2net.vspe). Then change the IP address in TcpClient device to the same as your **virtual machine**. Leave the port as it is. It should look something like this:

   [VSPE example](https://github.com/user-attachments/assets/5ab0cbd7-219e-4a6e-b750-aaa7aff8b42e)

10. After your ser2net is working, VSPE is set, it's time to run the Processing4.3 code. If it starts, it means that it has connected to the virtual machine via network.
11. In the last step, open two windows of your virtual machine. In the first one run Pimatic and in the second one, run the .c code VhduinoU like this:

		pimatic.js start

		./VhduinoU

13. Everything should be working great! Good luck!
