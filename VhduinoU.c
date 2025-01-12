/*
Copyright 2023 Kaunas University of technoly

by prof.dr. Egidijus Kazanavicius

		HOMEDUINO VIRTUAL BOARD

This Bord is simulated according description of HW produced in
Centre of Real time Computer Systems for students lab works and projects

Dependencies: VCP tty0tty     https://github.com/freemed/tty0tty
			  gcc compiler


Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the “Software”), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


#define SERIAL_PORT "/dev/tnt1" // Original serial port
#define FORWARD_PORT "/dev/tnt3" // Port to forward the data to

char buffer[255] = "                                    ";
char buffer1[255];
int bytes_read, bytes_read1;
char* cmd[20];
int len_cmd;
int wlen, wlen1;

int serial_fd;
int forward_fd; // File descriptor for the forward port
struct termios serial, serial2;

float temperature = 23.3, humidity = 50.5;

// Pin and state definitions
int dir_[22] = { 0 };
int sta[22] = { 0 };
int inp[22] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023 };

// Function to remove end-of-line characters from a string
void removeEOL(char* str) {
	size_t len = strlen(str);
	while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
		str[len - 1] = '\0';
		len--;
	}
}

// Function to write data to a serial port
int write_port(int fd, char* buffer, int size) {
	ssize_t result = write(fd, buffer, size);
	tcflush(fd, TCIFLUSH);
	if (result != size) {
		perror("failed to write to port");
		return -1;
	}
	return 0;
}


int read_port(int fd, char* buffer, int buffer_size, int timeout_usec) {
	fd_set fds;
	struct timeval timeout;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);

	timeout.tv_sec = 0;          // Seconds
	timeout.tv_usec = timeout_usec;    // Microseconds

	int ready = select(fd + 1, &fds, NULL, NULL, &timeout);

	if (ready == -1) {
		perror("select error");
		return -1;
	}
	else if (ready > 0) {
		if (FD_ISSET(fd, &fds)) {
			memset(buffer, 0, buffer_size);
			int bytes_read = read(fd, buffer, buffer_size - 1);

			if (bytes_read > 0) {
				return bytes_read;  // Data successfully read
			}
			else if (bytes_read == 0) {
				return 0;  // No data read
			}
			else {
				perror("Error reading from port");
				return -1;
			}
		}
	}

	return 0;  // Timeout
}

int read_serial() {
	int bytes_read = read_port(serial_fd, buffer, sizeof(buffer), 100000); // 100ms timeout
	if (bytes_read > 0) {
		if (write_port(forward_fd, buffer, bytes_read) == -1) {
			perror("Error forwarding data to /dev/tnt3");
		}
	}
	return bytes_read;
}

int read_forward_port() {
	return read_port(forward_fd, buffer1, sizeof(buffer1), 100000); // 100ms timeout
}


void argument_error(int fd) { write(fd, "ERR argument_error\r\n", strlen("ERR argument_error\r\n")); }
void unrecognized(int fd) { write(fd, "ERR unknown_command\r\n", strlen("ERR unknown_command\r\n")); }
void ack(int fd) { char* ack = "ACK\n"; write(fd, ack, strlen(ack)); }
void reset_(int fd) {
	//printf("RESET Ok\n");
	write_port(fd, "ready\r\n", strlen("ready\r\n"));
}

void ping(int fd) {
	//printf("PING Ok\n\r");
	write_port(fd, cmd[0], strlen(cmd[0]));
	if (cmd[1] != NULL) {
		write_port(fd, " ", strlen(" "));
		write_port(fd, cmd[1], strlen(cmd[1]));
	}
	write_port(fd, "\r\n", 2);
}

void digital_write(int fd) {
	if (len_cmd == 3)
	{

		int pin = atoi(cmd[1]);
		//printf("pin= %d \r\n", pin);
		//printf("dir= %d \r\n", dir_[pin]);
		if (pin > 14) { argument_error(fd); return; }
		int value = atoi(cmd[2]);
		if (value != 0 && value != 1) { argument_error(fd); return; }
		if (dir_[pin] == 1)
		{//ouput from state
			sta[pin] = value;
			//printf("sta[pin]= %d %d \r\n", pin, sta[pin]);
			inp[pin] = atoi(cmd[2]);
		}
		else {
			sta[pin] = value;
			//printf("_sta[pin]= %d %d \r\n", pin, sta[pin]);
		}
		char* ack = "ACK\r\n"; write_port(fd, ack, strlen(ack));
		//write(fd,"\r\n",2);

	}
	else unrecognized(fd);

}

void pm(int fd) {
	if (len_cmd == 3)
	{

		int pin = atoi(cmd[1]);
		if (pin > 20) { argument_error(fd); return; }
		int value = atoi(cmd[2]);
		if (value != 0 && value != 1) { argument_error(fd); return; }
		//printf("PM Ok\n");
		dir_[pin] = value;
		//printf("PM %d %d DIR = %d  Ok\r\n", pin, value, dir_[pin]);
		char* ack = "ACK "; write_port(fd, ack, strlen(ack));
		//write(fd," ",strlen(" "));
		write_port(fd, cmd[2], strlen(cmd[2]));
		write_port(fd, "\r\n", 2);

	}
	else unrecognized(fd);

}
void dht(int fd) {
	// Step 1: Read new temperature and humidity from the forward port
	int bytes_read = read_forward_port();
	if (bytes_read > 0) {
		int token_count = 0;
		char* tmp[20];
		char* token = strtok(buffer1, " ");
		while (token != NULL) {
			tmp[token_count++] = token; // Store each token in the array
			token = strtok(NULL, " ");    // Get the next token
		}

		// Ensure we received both temperature and humidity
		if (token_count == 3) {
			temperature = atof(tmp[1]);
			humidity = atof(tmp[2]);
			printf("Updated Temperature: %.2f, Humidity: %.2f from forward port\n", temperature, humidity);
		}
		else {
			printf("Invalid data format received from forward port\n");
		}
	}
	else if (bytes_read == 0) {
		printf("No data received from forward port\n");
	}
	else {
		perror("Error reading from forward port");
	}

	// Step 2: Process the DHT command (write temperature and humidity to serial port)
	char tmpt[20];
	char tmph[20];

	// Format the current temperature and humidity values into strings
	snprintf(tmpt, sizeof(tmpt), "%.2f", temperature);
	snprintf(tmph, sizeof(tmph), "%.2f", humidity);

	// Prepare and send acknowledgment along with the temperature and humidity
	char* ack = "ACK ";
	write_port(fd, ack, strlen(ack));
	write_port(fd, tmpt, (int)strlen(tmpt));
	write_port(fd, " ", strlen(" "));
	write_port(fd, tmph, (int)strlen("tmph"));
	write_port(fd, "\r\n", 2);

	// Log the values for debugging purposes
	//printf("DHT Command Processed: Temperature = %.2f, Humidity = %.2f\n", temperature, humidity);
}


void analog_read(int fd) {

	char tmp[20];

	if (len_cmd == 2)
	{

		int pin = atoi(cmd[1]);

		if (pin >= 14 && pin < 22 && dir_[pin] == 0)
		{
			//if (dir[pin] == 0 )
			//if (inp[pin] == 0) cmd[2] = "0\r\n"; else cmd[2] = "1\r\n";
			snprintf(tmp, sizeof(tmp), "%d", inp[pin]);
			char* ack = "ACK"; write(fd, ack, strlen(ack));
			write(fd, " ", strlen(" ")); write(fd, tmp, strlen(tmp));
			write(fd, "\r\n", 2);
		}
		else { argument_error(fd); return; }
	}
	else unrecognized(fd);
}
void digital_read(int fd) {
	if (len_cmd == 2)
	{

		int pin = atoi(cmd[1]);
		//printf("pin= %d \r\n", pin);
		//printf("dir= %d \r\n", dir_[pin]);
		if (pin > 20) { argument_error(fd); return; }
		if (dir_[pin] == 1) {//ouput from state
			char* ack = "ACK"; write(fd, ack, strlen(ack));
			if (sta[pin] == 0) cmd[2] = "0"; else cmd[2] = "1";
			write_port(fd, " ", strlen(" ")); write(fd, cmd[2], strlen(cmd[2]));
			write_port(fd, "\r\n", 2);
		}
		else {
			char* ack = "ACK "; write_port(fd, ack, strlen(ack));
			if (inp[pin] == 0) cmd[2] = "0\r\n"; else cmd[2] = "1\r\n";
			//write(fd," ",strlen(" "));
			write_port(fd, cmd[2], strlen(cmd[2]));
			//write(fd,"\r\n",2);
		}
	}
	else unrecognized(fd);
}
void analog_write(int fd) {
	// 3, 5, 6, 9, 10, 11 PWM pins
	if (len_cmd == 3)
	{
		int pin = atoi(cmd[1]);
		//printf("pin= %d \r\n", pin);
		if (pin != 3 && pin != 5 && pin != 6 && pin != 9 && pin != 11) { argument_error(fd); return; }
		int value = atoi(cmd[2]);
		if (cmd[2] == NULL) { argument_error(fd); return; }
		if (value > 256) { argument_error(fd); return; }
		char* ack = "ACK"; write(fd, ack, strlen(ack));
		write(fd, " ", strlen(" ")); write(fd, cmd[2], strlen(cmd[2]));
		write(fd, "\r\n", 2);
	}
	else unrecognized(fd);
}



int main() {


	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	char* token;
	char* xstr = "ready\r\n";
	int xlen = strlen(xstr);


	// Open the serial port
	serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_fd == -1) {
		perror("Error opening serial port");
		return EXIT_FAILURE;
	}

	// Configure the serial port settings
	memset(&serial, 0, sizeof(serial));
	serial.c_cflag = B115200 | CS8 | CREAD | CLOCAL; // Change B9600 to your baud rate if different
	serial.c_iflag = 0;
	serial.c_oflag = 0;
	serial.c_lflag = 0;
	tcflush(serial_fd, TCIFLUSH);
	tcsetattr(serial_fd, TCSANOW, &serial);

	// Open the serial port no2
	forward_fd = open(FORWARD_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (forward_fd == -1) {
		perror("Error opening serial port");
		return EXIT_FAILURE;
	}

	// Configure the serial port settings no2
	memset(&serial2, 0, sizeof(serial2));
	serial2.c_cflag = B115200 | CS8 | CREAD | CLOCAL; // Change B9600 to your baud rate if different
	serial2.c_iflag = 0;
	serial2.c_oflag = 0;
	serial2.c_lflag = 0;
	tcflush(forward_fd, TCIFLUSH);
	tcsetattr(forward_fd, TCSANOW, &serial2);

	while (1) {


		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(serial_fd, &read_fds);
		FD_SET(forward_fd, &read_fds);

		int max_fd = (serial_fd > forward_fd) ? serial_fd : forward_fd;

		struct timeval tv;
		tv.tv_sec = 0;       // No blocking, return immediately
		tv.tv_usec = 50000;  // Check every 50ms

		int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

		if (ready == -1) {
			perror("select");
			break;
		}

		if (FD_ISSET(serial_fd, &read_fds)) {
			// Read and process data from serial port
			wlen = read_serial();
			if (wlen > 0) {
				printf("Homeduino_cmd: %s", buffer);

				// Tokenize and process the command
				char* rest = buffer;
				removeEOL(rest);
				len_cmd = 0;
				while ((cmd[len_cmd] = strtok_r(rest, " ", &rest))) {
					len_cmd++;
				}

				// Command processing
				if (!strcmp(cmd[0], "RESET")) reset_(serial_fd);
				if (!strcmp(cmd[0], "PING")) ping(serial_fd);
				if (!strcmp(cmd[0], "DW")) digital_write(serial_fd);
				if (!strcmp(cmd[0], "PM")) pm(serial_fd);
				if (!strcmp(cmd[0], "DR")) digital_read(serial_fd);
				if (!strcmp(cmd[0], "AR")) analog_read(serial_fd);
				if (!strcmp(cmd[0], "AW")) analog_write(serial_fd);
				if (!strcmp(cmd[0], "DHT")) {
					dht(serial_fd);
					printf("Temperature = %f Humidity = %f \r\n", temperature, humidity);
				}
			}
		}

		if (FD_ISSET(forward_fd, &read_fds)) {
			// Read and process data from forward port
			wlen1 = read_forward_port();

			if (wlen1 > 0) {
				//printf("Forward data: %s\n", buffer1);

				// Process the forward port data
				char* rest1 = buffer1;
				removeEOL(rest1);
				int token_count = 0;
				char* tmp[20];
				char* token = strtok(buffer1, " ");
				while (token != NULL) {
					tmp[token_count++] = token; // Store each token in the array
					token = strtok(NULL, " ");    // Get the next token
				}
				//printf("TOKEN COUNT: %d\n", token_count);
				if (token_count == 2) {
					int state = atoi(tmp[1]);
					if (!strcmp(tmp[0], "0")) inp[14] = state;
					if (!strcmp(tmp[0], "1")) inp[15] = state;
					if (!strcmp(tmp[0], "2")) inp[3] = state;
					if (!strcmp(tmp[0], "3")) inp[17] = state;
				}
				else {
					printf("Invalid forward data format\n");
				}
				memset(buffer1, 0, sizeof(buffer1)); //reset buffer
			}
		}

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);

		struct timeval tv1;
		tv1.tv_sec = 0;
		tv1.tv_usec = 0;



		/*Code termination with 'q' pressed*/
		int readyButton = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv1);

		if (readyButton == -1) {
			perror("select");
			break;
		}
		else if (readyButton > 0) {
			if (FD_ISSET(STDIN_FILENO, &fds)) {
				char ch;
				read(STDIN_FILENO, &ch, 1);

				if (ch == 'q') {
					if (serial_fd > 0) close(serial_fd);
					if (forward_fd > 0) close(forward_fd);
					printf("Exiting...\n");
					break;
				}
			}
		}
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	write_port(serial_fd, xstr, xlen);

	// Close the serial port
	close(serial_fd);
	close(forward_fd);
	//return EXIT_SUCCESS;
}

