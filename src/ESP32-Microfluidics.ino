#include <WiFi.h>
#include <WebServer.h>
#include <string>
#include "Charzard.h"

// Replace with your network credentials
const char* ssid     = "YourInAComaWakeUp";
const char* password = "WakeUpNathan";

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output23State = "off";
String output22State = "off";

// Assign output variables to GPIO pins
const int output22 = 22;
const int output23 = 23;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//Main Loop Global Variables{
static int MRV = 9000; //max rotational velocity

int motorTurnVelocityRaw[8] = {0};
int motorTurnVelocity255[8] = {0};
bool checkBoxState[8] = {false}; //checkBoxState[0] never used
//}

//Fancy XML wizardry

char XML[200]; //XML buffer
char buf[32]; //buffer for char operations

IPAddress PageIP(192, 168, 1, 1); //IP address of the page
IPAddress gateway(192, 168, 1, 1); //IP address of the page
IPAddress subnet(255, 255, 255, 1); //IP address of the page
IPAddress ip; //IP address of the page

// Set web server port number to 80
// WiFiServer server(80); for WiFi.h OLD
WebServer server(80); // for WebServer.h to use on member for server class


void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output23, OUTPUT);
  pinMode(output22, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output23, LOW);
  digitalWrite(output22, LOW);

  //Disable watchdog timers to prevent resets
  // Might not need disabling, but play with in case
  disableCore0WDT(); // for network & sys tasks
  disableCore1WDT(); // for arduino sketch loops


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // on server http request execute function
  server.on("/", SendWebsite);
  server.on("/xml", SendXML);
  server.on("/SET_BUTTONS_ON", setButtonsOn);
  server.on("/SET_BUTTONS_OFF", setButtonsOff);
  server.on("/SET_MOTORS", setMotors);

  server.begin();
}

//functions
void killMotor( int num = -1 ){ //kill all motors or kills a spesified motor.
  if( num == -1 ){ //kills all motors
    analogWrite(output23, 0);
    analogWrite(output22, 0);
    //Void updateMotor( &stepper_driver_1, 0 );
    //Void updateMotor( &stepper_driver_2, 0 );
    //Void updateMotor( &stepper_driver_3, 0 );
    //Void updateMotor( &stepper_driver_4, 0 );
    //Void updateMotor( &stepper_driver_5, 0 );
    //Void updateMotor( &stepper_driver_6, 0 );
    //Void updateMotor( &stepper_driver_7, 0 );
    return;
  } //kills specified motor
  switch (num){
    case 1:
      analogWrite(output23, 0);
      //Void updateMotor( &stepper_driver_1, 0 );
      break;
    case 2:
      analogWrite(output22, 0);
      //Void updateMotor( &stepper_driver_2, 0 );
      break;
    case 3:
      //Void updateMotor( &stepper_driver_3, 0 );
      break;
    case 4:
      //Void updateMotor( &stepper_driver_4, 0 );
      break;
    case 5:
      //Void updateMotor( &stepper_driver_5, 0 );
      break;
    case 6:
      //Void updateMotor( &stepper_driver_6, 0 );
      break;
    case 7:
      //Void updateMotor( &stepper_driver_7, 0 );
      break;
  }
}

bool checkBoxToggleOff( int num ){
  killMotor(num);
  if( checkBoxState[num] == false )
    return true; //was false
  else
    checkBoxState[num] = false;
    return false;
}

bool checkBoxToggleOn( int num ){
  if( checkBoxState[num] == true )
    return true; //was true
  else
    checkBoxState[num] = true;
    return false;
}

void setButtonsOn(){
  //Serial.println("set buttons on");
  for( int i = 1; i < 8; i++){
    checkBoxToggleOn(i); //toggle all checkboxes on
  }
  server.send(200, "text/plain", ""); //Send web page
}

void setButtonsOff(){
  //Serial.println("set buttons on");
  for( int i = 1; i < 8; i++){
    checkBoxToggleOff(i); //toggle all checkboxes on
  }
  server.send(200, "text/plain", ""); //Send web page
}

void setMotors(){
  int MRVRaw = server.arg("MRVRaw").toInt();
  for( int i = 0; i < 8; i++ ){
    motorTurnVelocityRaw[i] = MRVRaw;
    motorTurnVelocity255[i] = map( motorTurnVelocityRaw[0], -MRV, MRV, 0, 255 );
  }
  server.send(200, "text/plain", ""); //Send web page
}

// MAIN LOOP
void loop(){
  server.handleClient(); //handle client requests
}

// Functions
void SendWebsite() {

  Serial.println("sending web page");
  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/html", PAGE_MAIN);
}


void SendXML() {

  // Serial.println("sending xml");

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  strcat(XML, "</Data>\n");
  // wanna see what the XML code looks like?
  // actually print it to the serial monitor and use some text editor to get the size
  // then pad and adjust char XML[2048]; above
  // Serial.println(XML);

  // you may have to play with this value, big pages need more porcessing time, and hence
  // a longer timeout that 200 ms
  server.send(200, "text/xml", XML);
}


void ProcessButton_0() {
  if(checkBoxState[0] == false){
    checkBoxToggleOn(0);
    Serial.print("Button 0 "); Serial.println(checkBoxState[0]);
    digitalWrite(output23, HIGH); //test
    server.send(200, "text/plain", "1"); //Send web page
  }
  else{
    checkBoxToggleOff(0);
    Serial.print("Button 0 "); Serial.println(checkBoxState[0]);
    digitalWrite(output23, LOW); //test
    server.send(200, "text/plain", "0"); //Send web page
  }
}


/*
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();   

      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;

        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK"); //I LOVE 200 OK :D
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Set all motor velocities to main velocity.
            if (header.indexOf("GET /setMotors") >= 0) {
              for( int i = 0; i < 8; i++ ){
                motorTurnVelocityRaw[i] = motorTurnVelocityRaw[0];
                motorTurnVelocity255[i] = map( motorTurnVelocityRaw[0], -MRV, MRV, 0, 255 );
              }
            }
            
            // Update checkbox#States (0 is motor all)
            if (header.indexOf("GET /setcheckbox1?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(1);
              } else { //unchecked
                checkBoxToggleOff(1);
              }
            }
            if (header.indexOf("GET /setcheckbox2?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(2);
              } else { //unchecked
                checkBoxToggleOff(2);
              }
            }
            if (header.indexOf("GET /setcheckbox3?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(3);
              } else { //unchecked
                checkBoxToggleOff(3);
              }
            }
            if (header.indexOf("GET /setcheckbox4?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(4);
              } else { //unchecked
                checkBoxToggleOff(4);
              }
            }
            if (header.indexOf("GET /setcheckbox5?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(5);
              } else { //unchecked
                checkBoxToggleOff(5);
              }
            }
            if (header.indexOf("GET /setcheckbox6?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(6);
              } else { //unchecked
                checkBoxToggleOff(6);
              }
            }
            if (header.indexOf("GET /setcheckbox7?state=") >= 0) {
              String stateString = header.substring(header.indexOf("state=") + 6);
              if( stateString.toInt() != 0 ){ //checked
                checkBoxToggleOn(7);
              } else { //unchecked
                checkBoxToggleOff(7);
              }
            }

            // on and off checkbox toggle logic
            if (header.indexOf("GET /setCheckBoxesOn") >= 0) {
              for( int i = 1; i < 8; i++){
                checkBoxToggleOn(i); //toggle all checkboxes on
              }
            }
            if (header.indexOf("GET /setCheckBoxesOff") >= 0) {
              for( int i = 1; i < 8; i++){
                checkBoxToggleOff(i); //toggle all checkboxes off. This doesn't kill motors.
                //killMotor(i); //if you want to kills all motors.
              }
            }
            
            // Update motorTurnVelocity (0 is motor all)
            if (header.indexOf("GET /setMRV0?value=") >= 0) {
              motorTurnVelocityRaw[0] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[0] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[0] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[0] < -MRV)
                motorTurnVelocityRaw[0] = -MRV;
              motorTurnVelocity255[0] = map( motorTurnVelocityRaw[0], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV1?value=") >= 0) {
              motorTurnVelocityRaw[1] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[1] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[1] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[1] < -MRV)
                motorTurnVelocityRaw[1] = -MRV;
              motorTurnVelocity255[1] = map( motorTurnVelocityRaw[1], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV2?value=") >= 0) {
              motorTurnVelocityRaw[2] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[2] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[2] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[2] < -MRV)
                motorTurnVelocityRaw[2] = -MRV;
              motorTurnVelocity255[2] = map( motorTurnVelocityRaw[2], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV3?value=") >= 0) {
              motorTurnVelocityRaw[3] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[3] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[3] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[3] < -MRV)
                motorTurnVelocityRaw[3] = -MRV;
              motorTurnVelocity255[3] = map( motorTurnVelocityRaw[3], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV4?value=") >= 0) {
              motorTurnVelocityRaw[4] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[4] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[4] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[4] < -MRV)
                motorTurnVelocityRaw[4] = -MRV;
              motorTurnVelocity255[4] = map( motorTurnVelocityRaw[4], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV5?value=") >= 0) {
              motorTurnVelocityRaw[5] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[5] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[5] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[5] < -MRV)
                motorTurnVelocityRaw[5] = -MRV;
              motorTurnVelocity255[5] = map( motorTurnVelocityRaw[5], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV6?value=") >= 0) {
              motorTurnVelocityRaw[6] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[6] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[6] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[6] < -MRV)
                motorTurnVelocityRaw[6] = -MRV;
              motorTurnVelocity255[6] = map( motorTurnVelocityRaw[6], -MRV, MRV, 0, 255 );
            }
            if (header.indexOf("GET /setMRV7?value=") >= 0) {
              motorTurnVelocityRaw[7] = header.substring(header.indexOf("value=") + 6).toInt();
              if (motorTurnVelocityRaw[7] > MRV) //fix -9000 to 9000 range limits
                motorTurnVelocityRaw[7] = MRV;      // MRV is a global static variable
              if (motorTurnVelocityRaw[7] < -MRV)
                motorTurnVelocityRaw[7] = -MRV;
              motorTurnVelocity255[7] = map( motorTurnVelocityRaw[7], -MRV, MRV, 0, 255 );
            }

            // Update & Run Specified Motors
            if (header.indexOf("GET /run") >= 0) {
              if( checkBoxState[1] == 1 ){
                //Void updateMotor( &stepper_driver_0, motorTurnVelocityRaw );
                Serial.println(motorTurnVelocity255[1]);
                analogWrite(output23, motorTurnVelocity255[1]);
              }
              if( checkBoxState[2] == 1 ){
                //Void updateMotor( &stepper_driver_1, motorTurnVelocityRaw );
                Serial.println(motorTurnVelocity255[2]);
                analogWrite(output22, motorTurnVelocity255[2]);
              }
              if( checkBoxState[3] == 1 ){
                //Void updateMotor( &stepper_driver_2, motorTurnVelocityRaw );
              }
              if( checkBoxState[4] == 1 ){
                //Void updateMotor( &stepper_driver_3, motorTurnVelocityRaw );
              }
              if( checkBoxState[5] == 1 ){
                //Void updateMotor( &stepper_driver_4, motorTurnVelocityRaw );
              }
              if( checkBoxState[6] == 1 ){
                //Void updateMotor( &stepper_driver_5, motorTurnVelocityRaw );
              }
              if( checkBoxState[7] == 1 ){
                //Void updateMotor( &stepper_driver_6, motorTurnVelocityRaw );
              }
            }

            if (header.indexOf("GET /kill") >= 0) {
              for( int i = 1; i < 8; i++){
                checkBoxState[i] == false; //checkBoxState[0] never used
              }
              killMotor(); //kills all motors
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the MRV control sliders
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;} ");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;} ");
            client.println(".slider { width: 300px; } "); 
            client.println(".myDiv { border: 4px outset lightblue; } ");
            client.println(".wrapper { display: flex; flex-wrap: wrap; justify-content: center; align-items: center; margin: 0 auto; } </style> ");

            // Web Page Heading
            client.println("<body><h1>Microfluidics Pump Controller</h1>");
            
            // All Motor Control (0)
            client.println("<p>Motor Rotational Velocity:</p>");
            client.println("<p style=\"font-size:10px;\">Accepted Range {" + String(-MRV) + ", " + String(MRV) + "}</p>");
            client.println("<div class=\"wrapper\">");
              client.println("<div class=\"myDiv\">");
                //set motors button
                client.println("<p><a href=\"/setMotors\"><button>Set All Motors</button></a></p>");

                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[0]) + "\" id=\"MotorRotationalVelocity0\" onchange=\"updateMRV0()\"></p>");
                client.println("<script>function updateMRV0() { var MRV0 = document.getElementById('MotorRotationalVelocity0').value; window.location.href = '/setMRV0?value=' + MRV0; }</script>");
              client.println("</div>");
              client.println("<div class=\"myDiv\">");
                //set checkboxes button ON
                client.println("<p><a href=\"/setCheckBoxesOn\"><button>Checkboxes On</button></a></p>");

                //set checkboxes button ON
                client.println("<p><a href=\"/setCheckBoxesOff\"><button>Checkboxes Off</button></a></p>");
              client.println("</div>");
            client.println("</div>");
            // Html Checkboxes & numBoxes for motors 0 -> 7
              // 1 (checkbox)
            client.println("<div class=\"wrapper\">");

              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[1]) + "\" id=\"checkbox1\" " + String(checkBoxState[1] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox1()\"> Motor 1</p>");
                client.println("<script>function togglecheckbox1() { var state = document.getElementById('checkbox1').checked ? 1 : 0; window.location.href = '/setcheckbox1?state=' + state; }</script>");
                 // 1 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[1]) + "\" id=\"MotorRotationalVelocity1\" onchange=\"updateMRV1()\"></p>");
                client.println("<script>function updateMRV1() { var MRV1 = document.getElementById('MotorRotationalVelocity1').value; window.location.href = '/setMRV1?value=' + MRV1; }</script>");
              client.println("</div>");

              // 2 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[2]) + "\" id=\"checkbox2\" " + String(checkBoxState[2] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox2()\"> Motor 2</p>");
                client.println("<script>function togglecheckbox2() { var state = document.getElementById('checkbox2').checked ? 1 : 0; window.location.href = '/setcheckbox2?state=' + state; }</script>");
                  // 2 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[2]) + "\" id=\"MotorRotationalVelocity2\" onchange=\"updateMRV2()\"></p>");
                client.println("<script>function updateMRV2() { var MRV2 = document.getElementById('MotorRotationalVelocity2').value; window.location.href = '/setMRV2?value=' + MRV2; }</script>");
              client.println("</div>");

              // 3 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[3]) + "\" id=\"checkbox3\" " + String(checkBoxState[3] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox3()\"> Motor 3</p>");
                client.println("<script>function togglecheckbox3() { var state = document.getElementById('checkbox3').checked ? 1 : 0; window.location.href = '/setcheckbox3?state=' + state; }</script>");
                  // 3 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[3]) + "\" id=\"MotorRotationalVelocity3\" onchange=\"updateMRV3()\"></p>");
                client.println("<script>function updateMRV3() { var MRV3 = document.getElementById('MotorRotationalVelocity3').value; window.location.href = '/setMRV3?value=' + MRV3; }</script>");
              client.println("</div>");

              // 4 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[4]) + "\" id=\"checkbox4\" " + String(checkBoxState[4] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox4()\"> Motor 4</p>");
                client.println("<script>function togglecheckbox4() { var state = document.getElementById('checkbox4').checked ? 1 : 0; window.location.href = '/setcheckbox4?state=' + state; }</script>");
                  // 4 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[4]) + "\" id=\"MotorRotationalVelocity4\" onchange=\"updateMRV4()\"></p>");
                client.println("<script>function updateMRV4() { var MRV4 = document.getElementById('MotorRotationalVelocity4').value; window.location.href = '/setMRV4?value=' + MRV4; }</script>");
              client.println("</div>");

              // 5 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[5]) + "\" id=\"checkbox5\" " + String(checkBoxState[5] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox5()\"> Motor 5</p>");
                client.println("<script>function togglecheckbox5() { var state = document.getElementById('checkbox5').checked ? 1 : 0; window.location.href = '/setcheckbox5?state=' + state; }</script>");
                  // 5 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[5]) + "\" id=\"MotorRotationalVelocity5\" onchange=\"updateMRV5()\"></p>");
                client.println("<script>function updateMRV5() { var MRV5 = document.getElementById('MotorRotationalVelocity5').value; window.location.href = '/setMRV5?value=' + MRV5; }</script>");
              client.println("</div>");

              // 6 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[6]) + "\" id=\"checkbox6\" " + String(checkBoxState[6] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox6()\"> Motor 6</p>");
                client.println("<script>function togglecheckbox6() { var state = document.getElementById('checkbox6').checked ? 1 : 0; window.location.href = '/setcheckbox6?state=' + state; }</script>");
                  // 6 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[6]) + "\" id=\"MotorRotationalVelocity6\" onchange=\"updateMRV6()\"></p>");
                client.println("<script>function updateMRV6() { var MRV6 = document.getElementById('MotorRotationalVelocity6').value; window.location.href = '/setMRV6?value=' + MRV6; }</script>");
              client.println("</div>");

              // 7 (checkbox)
              client.println("<div class=\"myDiv\">");
                client.println("<p><input type=\"checkbox\" value=\"" + String(checkBoxState[7]) + "\" id=\"checkbox7\" " + String(checkBoxState[7] ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox7()\"> Motor 7</p>");
                client.println("<script>function togglecheckbox7() { var state = document.getElementById('checkbox7').checked ? 1 : 0; window.location.href = '/setcheckbox7?state=' + state; }</script>");
                  // 7 (textbox)
                client.println("<p><input type=\"number\" min=\"" + String(-MRV) + "\" max=\"" + String(MRV) + "\" value=\"" + String(motorTurnVelocityRaw[7]) + "\" id=\"MotorRotationalVelocity7\" onchange=\"updateMRV7()\"></p>");
                client.println("<script>function updateMRV7() { var MRV7 = document.getElementById('MotorRotationalVelocity7').value; window.location.href = '/setMRV7?value=' + MRV7; }</script>");
              client.println("</div>");
            
            client.println("</div>"); //wrapper


            // Execute Button
            client.println("<p><a href=\"/run\"><button class=\"button\" style=\"color: MediumSeaGreen\">Run</button></a></p>");

            // Kill Button
            client.println("<p><a href=\"/kill\"><button class=\"button\" style=\"color: Tomato\";>Kill</button></a></p>");

            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
*/