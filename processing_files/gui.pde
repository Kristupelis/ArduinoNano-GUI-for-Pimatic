import controlP5.*; // Include controlP5 library //<>//
import processing.serial.*; // Import serial communication
import java.util.Arrays;

Serial port;
String buffer = ""; // Buffer to store received data
ControlP5 cp5; // Create ControlP5 object
PFont font;
// 0 - D0(RX0), 1 - D1(TX1),..., 14 - D14(A0),..., 21 - D21(A7)
int[] pinColors = new int[22]; // Colors for the 4 buttons
//0 - D13 LED, 1 - PWR, 2 - TX, 3 - RX
int[] ledColors = new int[4];

//1-output, 0-input
int[] pinDirections = {0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
int[] pinStates = {0,0,0,0,0,0,0,0,0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1023, 1023}; // Store the states of each pin
int[] pinInputs ={1,1,1,1,1,1,1,1,1,1, 1, 1, 1, 1, 
// 14    15    16    17    18    19    A6    A7
  1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};
  
//String[] pinLabels = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7"};
String[] pinLabels = {"0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","1023","1023"};

boolean[] ledStates = new boolean[4]; // Store the states of each LED
boolean toggleValue = false; // Toggle value initialization

int isRunning = 0;
boolean manualToggle = true;
boolean[] buttonHoverStates = new boolean[4];
int[] buttonColors = new int[4];
int[] hoverColors = {color(100, 200, 255), color(150, 250, 150), color(255, 200, 100), color(255, 150, 150)};
boolean[] buttonStates = {false, false, false, false};

PImage img, img11;

float temperature = 23.3;
float humidity = 50.5;

void setup() {
  Arrays.fill(pinColors, 255);
  Arrays.fill(ledColors, 255);
  size(500, 800); // Window width and height
  img = loadImage("arduino1.jpg");
  img11 = loadImage("DHT11.jpg");
  
  font = createFont(PFont.list()[2], 14); // Font for button and title
  printArray(Serial.list()); // Print available serial ports
  
  port = new Serial(this, "COM1", 9600); // COM port for Arduino (adjust as needed)
  
  cp5 = new ControlP5(this); // Initialize ControlP5
  smooth();

  // Create toggles and sliders
  for (int i = 0; i < 4; i++) {
  cp5.addToggle("toggle" + (i + 1))
    .setPosition(180, 60 + i * 100)
    .setSize(100, 40)
    .setValue(true)
    .setMode(ControlP5.SWITCH)  // Set toggle mode to SWITCH
    .setColorActive(color(0)) // Grey color for the switch when active
    .setColorForeground(color(200)) // Grey border for the toggle
    .setColorBackground(color(230)) // Black background for the toggle
    .setColorLabel(color(255)); // White label text
}
  // Create four buttons to the right of the toggles
   for (int i = 0; i < 4; i++) {
    int xPos = 300 + (i % 2) * 100;  // Alternate x position for buttons
    int yPos = 60 + (i / 2) * 120;   // Stack buttons vertically

    cp5.addButton("button" + (i + 1)) // Rename buttons to button1, button2, etc.
      .setPosition(xPos, yPos)
      .setSize(80, 80)
      .setLabel("K " + i)
      .setColorBackground(color(255, 0, 0));
  }

  cp5.addSlider("Temperature").setPosition(20, 450).setSize(200, 30).setRange(-30, 50).setColorBackground(color(200))
    .setColorForeground(color(100, 150, 255));
  cp5.addSlider("Humidity").setPosition(20, 500).setSize(200, 30).setRange(0, 100).setColorBackground(color(200))
    .setColorForeground(color(100, 150, 255));
}

void draw() {
  background(255); // Background color
  fill(0); // Text color
  
  textFont(font);
  text("LED CONTROL GUI", 155, 30);
  String[] labels = {"LED12", "LED11", "LED10", "SW13", "Air", "Temperature", "Humidity"};
  
  for (int i = 0; i < 4; i++) {
    text(labels[i], 60, 80 + i * 100);
  }
  text(labels[4], 50, 425); // "Air"
  //text(labels[5], 330, 475); // "Temperature"
  //text(labels[6], 300, 525); // "Humidity"
  // Display slider values
  temperature = cp5.getController("Temperature").getValue();
  humidity = cp5.getController("Humidity").getValue();
  textAlign(LEFT);
  text("Temperature: " + nf(temperature, 1, 1) + " °C", 230, 480);
  text("Humidity: " + nf(humidity, 1, 1) + " %", 230, 530);

  // Draw images
  //image(img, 0, 430);
  image(img, 14, 583);
  img11.resize(100, 140);
  //image(img11, 350, 650);
  image(img11, 380, 420);

  // Draw LED indicators and pins
  //                           D0          D1          D2         D3           D4          D5         D6           D7         D8          D9         D10         D11        D12     
  int[][] pinPositions = {{408, 595}, {432, 595}, {331, 595}, {306, 595}, {280, 595}, {254, 595}, {228, 595}, {201, 595}, {176, 595}, {150, 595}, {125, 595}, {98, 595}, {73, 595}
  //     D13         D14        D15        D16        D17       D18         D19        D20        D21
    , {73, 749}, {150, 749}, {176, 749}, {202, 749}, {227, 749}, {253, 749}, {278, 749}, {303, 749}, {330, 749}};
  int[][] ledPositions = {{315, 716}, {315, 679}, {315, 646}, {315, 616}};
  
  textFont(createFont(PFont.list()[2], 10));
  for (int i = 0; i < pinPositions.length; i++) {
        drawPin(pinPositions[i][0], pinPositions[i][1], pinColors[i]);

        // Draw text labels with rotation
        fill(0); // Set text color to black
        pushMatrix(); // Save the current transformation matrix
        
        // Translate to the pin position
        translate(pinPositions[i][0], pinPositions[i][1]);
        
        // Rotate the text 90 degrees (PI / 2 radians)
        //rotate(-PI/2);
        
        // Adjust text position to be centered below or above the pin
        
        if (i < 13) {
          rotate(-PI / 2);  
            textAlign(LEFT, CENTER);
            text(pinLabels[i], +15, 0); // Draw above the pin
        } else {
            rotate(-PI / 2);  
            textAlign(RIGHT, CENTER);
            text(pinLabels[i], -18, 0); // Draw below the pin
        }
        
        popMatrix(); // Restore the previous transformation matrix
    }
  //textFont(font);
  for (int i = 0; i < ledPositions.length; i++) {
    drawLed(ledPositions[i][0], ledPositions[i][1], ledColors[i]);
  }
  
  // Read slider values
  temperature = cp5.getController("Temperature").getValue();
  humidity = cp5.getController("Humidity").getValue();
}

void drawPin(int x, int y, int col) {
  pushMatrix();
  translate(x, y);
  fill(col);
  ellipse(0, 0, 15, 15);
  popMatrix();
}
void drawLed(int x, int y, int col) {
  pushMatrix();
  translate(x, y);
  fill(col);
  rect(0, 0, 25, 12);
  popMatrix();
}

// Function to control LEDs
void toggle(boolean state, int index, char onCmd, char offCmd) {
  //port.write(state ? offCmd : onCmd);
  pinColors[index] = state ? color(255) : color(0, 255, 0);
}

void toggle1(boolean state) { toggle(state, 12, 'a', 'x'); }
void toggle2(boolean state) { toggle(state, 11, 'b', 'y'); }
void toggle3(boolean state) { toggle(state, 10, 'c', 'z'); }
void toggle4(boolean state) { toggle(state, 13, 'o', 'f'); 
toggleLed(0, !state, '1', '2');}

// Serial event handler
void serialEvent(Serial myPort) {
  while (myPort.available() > 0) {
    char received = myPort.readChar();
    buffer += (received != '\n') ? received : "";
    if (received == '\n') {
      processData(buffer);
      buffer = "";
    }
    
  }
}

// Function to process received data
void processData(String data) {
  String[]words = data.split(" ");
  if(words.length == 3)
  {
    
  }
  switch(words[0]) {
    case "DHT":
    case "dht":
      String tmp = nf(temperature, 0, 2);
      String hmd = nf(humidity, 0, 2);
      println("DHT command sent back");
      port.write("dht " + tmp + " " + hmd);
      //port.write("\n");
      //port.write("Humidity: " + hmd);
      //port.write("\n");
      /*
      temperature += 0.1;
      if (temperature > 27) temperature = 23.3;
      humidity -= 0.1;
      if(humidity < 45) humidity = 53.3;
      */
      break;
    case "DW":
    case "dw":
    if(words.length != 3)
    {
      println("Not enought or to many arguments for DW");
      break;
    }
    int state = Integer.valueOf(words[2]);
    int index = Integer.valueOf(words[1]);
    int invertedState = (state == 1) ? 0 : 1;
    if(index > 14)
    {
      println("Wrong PIN number for DW");
      break;
    }
    if(index == 13)
    {
      manualToggle = false;
      cp5.getController("toggle" + 4).setValue(invertedState);
    }
    if(index == 12)
    {
      manualToggle = false;
      cp5.getController("toggle" + 1).setValue(invertedState);
    }
    if(index == 11)
    {
      manualToggle = false;
      cp5.getController("toggle" + 2).setValue(invertedState);
    }
    if(index == 10)
    {
      manualToggle = false;
      cp5.getController("toggle" + 3).setValue(invertedState);
    }
    stateCheck(state, index);
    if(pinDirections[index] == 1)
    {
      pinInputs[index] = state;
    }
    break;
    case "DR":
    case "dr":
    if(words.length != 2)
    {
      println("Not enought or to many arguments for DR");
      break;
    }
    index = Integer.valueOf(words[1]);
    if(index > 20)
    {
      println("Wrong PIN for DR");
      break;
    }
    if(pinDirections[index] == 1)
    {
      if(pinStates[index] == 0) {
        println("State of PIN " + index +" is OFF");
      }
      else {
        println("State of PIN " + index +" is ON");
      }
    }
    else
    {
      if(pinInputs[index] == 0) {
        println("State of PIN " + index +" is OFF");
      }
      else {
        println("State of PIN " + index +" is ON");
      }
    }
    break;
    case "AR":
    case "ar":
    if(words.length != 2)
    {
      println("Not enought or to many arguments for AR");
      break;
    }
    index = Integer.valueOf(words[1]);
    if(index >= 14 && index <=21 && pinDirections[index] == 0) //nuo A0 iki A7
    {
      println("The value of PIN is " + pinInputs[index]);
    }
    else println("Wrong PIN for AR");
    break;
    case "AW":
    case "aw":
    if(words.length != 3)
    {
      println("Not enought or to many arguments for AW");
      break;
    }
    index = Integer.valueOf(words[1]);
    int value =  Integer.valueOf(words[2]);
    if (index != 3 && index != 5 && index != 6 && index != 9 && index != 10 && index != 11)
    {
      println("Wrong PIN for AW");
      break;
    }
    if(value < 0 || value > 256)
    {
      println("Incorrect value for AW");
      break;
    }
    //finish the code
    //pinStates[index] = value; 
    break;
    case "PM":
    case "pm":
    if(words.length != 3)
    {
      println("Not enought or to many arguments for PM");
      break;
    }
    index = Integer.valueOf(words[1]);
    int mode = Integer.valueOf(words[2]);
    if(index > 20)
    {
      println("Wrong PIN for PM");
      break;
    }
    if(mode !=1 && mode !=0)
    {
      println("Wrong value for PM");
      break;
    }
    pinDirections[index] = mode;
    break;
    default: 
    println("Wrong command");
    break;
  }
  if (data.equals("DW 13 1")) {
    togglePin(13, true, '1','2');
    toggleLed(0, true, '1', '2');
  } else if (data.equals("DW 13 0")) {
    togglePin(13, false, '0','2');
    toggleLed(0, false, '0', '2');
  }
}

void togglePin(int index, boolean state, char onCmd, char offCmd) {
  //port.write(state ? onCmd : offCmd);
  pinColors[index] = state ? color(0, 255, 0) : color(255);
  if(state == true)
    pinStates[index] = 1;
  else
    pinStates[index] = 0;
}

void toggleLed(int index, boolean state, char onCmd, char offCmd) {
  //port.write(state ? onCmd : offCmd);
  ledColors[index] = state ? color(255, 204, 0) : color(255);
  ledStates[index] = state;
}

void stateCheck (int state, int index) {
  if(state == 1){
        togglePin(index, true, '1','2');
      }
      else if(state== 0){
        togglePin(index, false, '1','2');
      }
      else
        println("Wrong state - should be 0 or 1");
}
void toggleButtonColor(int index) {
  buttonStates[index] = !buttonStates[index]; // Toggle the button state
  int newColor = buttonStates[index] ? color(0, 255, 0) : color(255, 0, 0); // Green if true, red if false
  cp5.getController("button" + (index + 1)).setColorBackground(newColor); // Set button color
  //port.write("K" + index + ": " + (buttonStates[index] ? "1\n" : "0\n"));
  port.write(index + " " + (buttonStates[index] ? "0\n" : "1\n"));
  if(index == 0){
    pinInputs[14] = ~pinInputs[14] & 1;
  }
  if(index == 1){
    pinInputs[15] = ~pinInputs[15] & 1;
  }
  if(index == 2){
    pinInputs[3] = ~pinInputs[3] & 1;
  }
  if(index == 3){
    pinInputs[17] = ~pinInputs[17] & 1;
  }
}
void mousePressed() {
  for (int i = 0; i < 4; i++) {
    Button button = (Button) cp5.getController("button" + (i + 1));
    if (button.isMouseOver()) {
      toggleButtonColor(i);
    }
  }
}
// Update toggle function
void controlEvent(ControlEvent theEvent) {
  if (theEvent.isFrom("toggle1") || theEvent.isFrom("toggle2") || theEvent.isFrom("toggle3") || theEvent.isFrom("toggle4")) {
    if (theEvent.getController().isMouseOver()) manualToggle = true;
    int toggleIndex = Integer.parseInt(theEvent.getName().replace("toggle", ""));
    boolean toggleState = theEvent.getController().getValue() == 1;
    if(toggleIndex == 4) toggleIndex = 13;
    if(toggleIndex == 3) toggleIndex = 10;
    if(toggleIndex == 2) toggleIndex = 11;
    if(toggleIndex == 1) toggleIndex = 12;
    if (manualToggle == true) { handleToggle(toggleIndex, toggleState); }
  }
}

// Handle toggle state change
void handleToggle(int index, boolean state) {
  // Generate the command string
  String command = index + " " + (state ? "0" : "1");
  //println("Sending to port: " + command); // Debugging output
  //println("DW " + command);
  if(isRunning > 3)
    port.write("DW " + command + "\n"); // Write to serial port with a newline
    isRunning++;
}
