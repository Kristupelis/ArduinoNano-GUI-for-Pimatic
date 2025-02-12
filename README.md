# ArduinoNano-GUI-for-Pimatic
This project was made to show off how ArduinoNano module behaves when it's connected to Pimatic

--------------------------------------------------------------------------------------------

To understand the project and its needs, we have to understand how everything works. First of all, we have a Linux virtual machine, which has a .c code that simulates Homeduino protocol commands such as "DW 13 1", "AR 7" and similar on ArduinoNano module. These commands are being sent to Pimatic service via serial port pairs /dev/tnt0 <> /dev/tnt1. How to set up such serial ports you can read: https://github.com/freemed/tty0tty. Then in pimatic we can see the simulated temperature, humidity and other incoming commands. However, we are not only receiving, but also sending commands from Pimatic to the .c code when for example we flick a switch in Pimatic GUI. That's how we communicate with serial devices.

The problem is that we would have to have a physical ArduinoNano module, DHT senor, buttons and other components and connect them to our computer to see, how these componets behave when we press buttons, turn on switches or do something else. To solve this problem, I made a Virtual ArduinoNano module, DHT sensor and whatever else you want to have, since the code can be freely adjusted by your needs. For example you can even have 1000 pins if that's your desire:).

This solution works with ser2net technology (more information about it in: https://github.com/cminyard/ser2net). What I did first was I created a GUI for ArduinoNano and other components using Processing4.3 (Java programming language). It's a very useful tool if you want to create something visually working. This Processing4.3 code also connects to my personal computer virtual port COM1 which was created using VSPE. The only thing left was to update my .c code to not only send and receive data from Pimatic, but also send and receive data from my personal computers Processing4.3 code. For this connection I created a ser2net service (TCP network bridge), changed .yaml and .conf files and that's it. I just had to connect my new /dev/tnt pair with my virtual port on my personal computer via network.

Project scheme:

![Project Scheme](https://github.com/user-attachments/assets/7825414a-2c95-43c4-b7c1-e09815be5d1d)


--------------------------------------------------------------------------------------------

Steps to run this solution:
1. Install ser2net

		sudo apt install ser2net -y

2. Verify installation:

		ser2net --version
   
3. Update your ser2net.conf and ser2net.yaml files in /etc folder with (or just download them from this repository and replace the old ones):

     	sudo nano /etc/ser2net.conf
   		sudo nano /etc/ser2net.yaml

4. Enalbe/restart/stop or check status for ser2net service (don't forget to restart ser2net if you changed .yaml or .conf files):

		sudo systemctl enable ser2net
		sudo systemctl restart ser2net
		sudo systemctl start ser2net
		sudo systemctl status ser2net

5. After running "sudo systemctl status ser2net" you should see something like this:

   ![ser2net status example](https://github.com/user-attachments/assets/bc2d6348-4de9-4367-9127-5e130abfc694)

6. Copy VhduinoU.c file from your Windows computer to your virtual machine with:

		scp C:\Users\[YourWindowsUsername]\Desktop\Patobulinta\VhduinoU.c root@158.129.30.xxx:/root

7. Compile this C code on your virtual machine with:

		gcc -o VhduinoU VhduinoU.c

8. Open VSPE and load the configuration file (ser2net.vspe). You can dowload VSPE here: https://eterlogic.com/Products.VSPE_Download.html. Then change the IP address in TcpClient device to the same as your **virtual machine**. Leave the port as it is (2000). It should look something like this:

   ![VSPE example](https://github.com/user-attachments/assets/5ab0cbd7-219e-4a6e-b750-aaa7aff8b42e)

8.1 If you need to configure VSPE by your self, then here are 2 photos which show how to set up Connector and TcpClient components:

[ConnectorComponent](https://github.com/user-attachments/assets/4c242bcb-6b31-46d9-b4e1-5af6e09ea486)
[TcpClientComponent](https://github.com/user-attachments/assets/f4f728da-b968-4d4c-a6f1-1893d2153ed6)

9. After your ser2net is working, VSPE is set, it's time to run the Processing4.3 code. If it starts, it means that it has connected to the virtual machine via network.
10. In the last step, open two windows of your virtual machine. In the first one run Pimatic and in the second one, run the .c code VhduinoU like this:

		pimatic.js start

		./VhduinoU

11. Everything should be working great! Good luck!
--------------------------------------------------------------------------------------------
NOTICE: there needs to be some work done to fullfil this projects' full potential. What I mean by that is that the buttons in Proccesing4.3 GUI don't "work" - after clicking them their behaviour doesn't reflect in Pimatic GUI, while clicking switches/buttons in Pimatic the behaviour is reflected in Processing4.3. If anyone finishes editing VhudinoU.c code and completes this functionallity, you can message me through LinkedIn (https://www.linkedin.com/in/kristupas-cilcius-052b27252/) and I will add you to this project as a contributor.
