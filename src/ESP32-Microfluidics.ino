#include <WiFi.h> // contains WiFi class which creates a WiFi object. Simplifies the connection to a WiFi network.
#include <WebServer.h> // contains WebServer class which creates a server object. Simplifies the connection to a web server.
#include "Charzard.h" // contains the HTML code and JS logic of the web server.

// The ssid and password are the typical wifi or network connection credentials
const char* ssid     = "YourInAComaWakeUp";
const char* password = "WakeUpNathan";

// GPIO pins vairables
const int output22 = 22;
const int output23 = 23;

// Motor state variables
const int MRV = 90000; //max rotational velocity
int motorTurnVelocityRaw[8] = {0}; //Raw is value from -MRV to MRV
int motorTurnVelocity255[8] = {0}; //225 is the Raw mapped to 0-255 for GPIO PWM
bool checkBoxState[8] = {false}; //checkBoxState[0] never used

// Set web server object with port number
WebServer server(80);

void setup() {
  Serial.begin(115200); // set serial baud rate to 115200 
  // Initialize and set GPIO
  pinMode(output23, OUTPUT);
  pinMode(output22, OUTPUT);
  digitalWrite(output23, LOW);
  digitalWrite(output22, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to Wi-Fi network with SSID with password
  WiFi.begin(ssid, password);
  
  //wait till connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server. (put ip into browser to view server)
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  /* on server http request execute function
     server.handleClient() compares request to
     first parameter and then executes the function
     in the second parameter. */
  // Send webpage
  server.on("/", SendWebsite); //fowardslash is sent by the website on web ip load.
  //Modify ALL motors checkbox requests
  server.on("/SET_ALL_MOTORS", setMotors); //checkbox 0 is all motors
  server.on("/UPDATE_MOTOR_ALL_CONTROL", setMotor0); //used to set all motors to a specific value
  server.on("/SET_CHECKBOXES_ON", setCheckboxesOn);
  server.on("/SET_CHECKBOXES_OFF", setCheckboxesOff);
  //Modify motors checkboxs individually requests
  server.on("/SET_CHECKBOX1", checkBox1Toggle);
  server.on("/SET_CHECKBOX2", checkBox2Toggle);
  server.on("/SET_CHECKBOX3", checkBox3Toggle);
  server.on("/SET_CHECKBOX4", checkBox4Toggle);
  server.on("/SET_CHECKBOX5", checkBox5Toggle);
  server.on("/SET_CHECKBOX6", checkBox6Toggle);
  server.on("/SET_CHECKBOX7", checkBox7Toggle);
  //Modify motors velocity requests
  server.on("/SET_MRV1", setMotor1);
  server.on("/SET_MRV2", setMotor2);
  server.on("/SET_MRV3", setMotor3);
  server.on("/SET_MRV4", setMotor4);
  server.on("/SET_MRV5", setMotor5);
  server.on("/SET_MRV6", setMotor6);
  server.on("/SET_MRV7", setMotor7);
  //Run motors requests
  server.on("/RUN", runMotors);
  server.on("/KILL", killMotors);
  //Start Server
  server.begin();
}

// MAIN LOOP
void loop(){
  //points client requests to server.on functions declared in setup().
  //This function will call server.on("/", SendWebsite) until a server is connected.
  server.handleClient(); 
}

// send server inital connection by server.on("/", SendWebsite)
void SendWebsite() {
  Serial.println("sending web page");
  server.send(200, "text/html", PAGE_MAIN); //Send the html gui to the client
}

/*  This function is used to set, run, or kill a motor.
    To set a motor use setMotorNumRunKill( num, value, false, false )
    To run a motor use setMotorNumRunKill( num, value, true, false )
    To kill a motor use:
      setMotorNumRunKill( num, value, false, true )
      setMotorNumRunKill( num, value, true, true )
    num:      Motor numbers [0,7] where 0 is for set all motors
    value:    The raw motor rotational velocity. Range [-MRV, MRV]
    run:      Runs the motor specified.
    kill:     false, updates the motorTurnVelocityRaw
              true, keeps the previous motorTurnVelocityRaw and sets motor to 0
              regardless of value.*/
void setMotorNumRunKill( int num, int value, int run = false, int kill = false ){
  switch (num)
  {
    case 0:
      motorTurnVelocityRaw[0] = value;
      break;
    case 1:
      if(kill == true){
        //void updateMotor( &stepper_driver_1, 0 );
        analogWrite(output22, 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[1] = value;
      motorTurnVelocity255[1] = map( motorTurnVelocityRaw[1], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_1, motorTurnVelocityRaw[1] );
        analogWrite(output22, motorTurnVelocity255[1]);
      }
      break;
    case 2:
      if(kill == true){
        //void updateMotor( &stepper_driver_2, 0 );
        analogWrite(output23, 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[2] = value;
      motorTurnVelocity255[2] = map( motorTurnVelocityRaw[2], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_2, motorTurnVelocityRaw[2] );
        analogWrite(output23, motorTurnVelocity255[2]);
      }
      break;
    case 3:
      if(kill == true){
        //void updateMotor( &stepper_driver_3, 0 );
        break;
      }// else kill == false
      motorTurnVelocityRaw[3] = value;
      //void updateMotor( &stepper_driver_3, motorTurnVelocityRaw[3] );
      break;
    case 4:
      if(kill == true){
        //void updateMotor( &stepper_driver_4, 0 );
        break;
      }// else kill == false
      motorTurnVelocityRaw[4] = value;
      //void updateMotor( &stepper_driver_4, motorTurnVelocityRaw[4] );
      break;
    case 5:
      if(kill == true)
        //void updateMotor( &stepper_driver_5, 0 );
        break;
      motorTurnVelocityRaw[5] = value;
      //void updateMotor( &stepper_driver_5, motorTurnVelocityRaw[5] );
      break;
    case 6:
      if(kill == true)
        //void updateMotor( &stepper_driver_6, 0 );
        break;
      motorTurnVelocityRaw[6] = value;
      //void updateMotor( &stepper_driver_6, motorTurnVelocityRaw[6] );
      break;
    case 7:
      if(kill == true)
        //void updateMotor( &stepper_driver_7, 0 );
        break;
      motorTurnVelocityRaw[7] = value;
      //void updateMotor( &stepper_driver_7, motorTurnVelocityRaw[7] );
      break;
  }
}

//functions
void checkBoxToggleOff( int num ){
  setMotorNumRunKill(num, 0, true, true); //kill motor if it was on
  //toggle checkbox to off
  if( checkBoxState[num] == true )
    checkBoxState[num] = false;
}

void checkBoxToggleOn( int num ){
  //toggle checkbox to on
  if( checkBoxState[num] == false )
    checkBoxState[num] = true;
}

void setCheckboxesOn(){
  //Serial.println("set buttons on");
  for( int i = 1; i < 8; i++){
    checkBoxToggleOn(i); //toggle all checkboxes on
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setCheckboxesOff(){
  //Serial.println("set checkboxes off");
  for( int i = 1; i < 8; i++){
    setMotorNumRunKill(i, 0, true, true); //toggle all checkboxes off
    checkBoxToggleOff(i); //toggle all checkboxes off
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotors(){
  for( int i = 1; i < 8; i++ ){
    setMotorNumRunKill( i, motorTurnVelocityRaw[0] ); //set all motors to the same value
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

//checkbox invidual toggles
void checkBox1Toggle(){
  bool checkState = (server.arg("STATE") == "true"); //return string of js bool
  checkBoxState[1] = checkState;
  if(checkState == false)
    setMotorNumRunKill(1, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox2Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[2] = checkState;
  if(checkState == false)
    setMotorNumRunKill(2, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox3Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[3] = checkState;
  if(checkState == false)
    setMotorNumRunKill(3, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox4Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[4] = checkState;
  if(checkState == false)
    setMotorNumRunKill(4, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox5Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[5] = checkState;
  if(checkState == false)
    setMotorNumRunKill(5, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox6Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[6] = checkState;
  if(checkState == false)
    setMotorNumRunKill(6, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

void checkBox7Toggle(){
  bool checkState = (server.arg("STATE") == "true");
  checkBoxState[7] = checkState;
  if(checkState == false)
    setMotorNumRunKill(7, 0, true, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

//motors individual velocity setters
void setMotor0(){
  int MRVRaw = server.arg("VALUE").toInt(); //return string of js int
  setMotorNumRunKill(0, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor1(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(1, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor2(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(2, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor3(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(3, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor4(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(4, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor5(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(5, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor6(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(6, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor7(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRunKill(7, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

//run motors that are checked
void runMotors(){
  for( int i = 1; i < 8; i++){
    if( checkBoxState[i] == true ){
      setMotorNumRunKill(i, motorTurnVelocityRaw[i], true);
    }
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

// kills all motors.
void killMotors(){
  for( int i = 1; i < 8; i++){
    setMotorNumRunKill(i, 0, true, true); //num, value, run, kill (kill takes priority)

  }
  server.send(200, "text/plain", ""); //Send web page
}