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

int MRV = 90000; //max rotational velocity

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
  //Modify ALL motors checkbox requests
  server.on("/SET_ALL_MOTORS", setMotors); //checkbox 0 is all motors
  server.on("/UPDATE_MOTOR_ALL_CONTROL", setMotor0); //used in set_all_motors to set all motors to this value
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
  server.send(200, "text/html", PAGE_MAIN); //Sending web page connection
}

void setMotorNumRun( int num, int value, int run = false ){
  switch (num)
  {
    case 0:
      motorTurnVelocityRaw[0] = value;
      motorTurnVelocity255[0] = map( motorTurnVelocityRaw[0], -MRV, MRV, 0, 255 );
      break;
    case 1:
      motorTurnVelocityRaw[1] = value;
      motorTurnVelocity255[1] = map( motorTurnVelocityRaw[1], 0, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_1, motorTurnVelocityRaw[1] );
        analogWrite(output22, motorTurnVelocity255[1]);
      }
      break;
    case 2:
      motorTurnVelocityRaw[2] = value;
      motorTurnVelocity255[2] = map( motorTurnVelocityRaw[2], 0, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_2, motorTurnVelocityRaw[2] );
        analogWrite(output23, motorTurnVelocity255[2]);
      }
      break;
    case 3:
      motorTurnVelocityRaw[3] = value;
      motorTurnVelocity255[3] = map( motorTurnVelocityRaw[3], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_3, motorTurnVelocityRaw[3] );
      break;
    case 4:
      motorTurnVelocityRaw[4] = value;
      motorTurnVelocity255[4] = map( motorTurnVelocityRaw[4], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_4, motorTurnVelocityRaw[4] );
      break;
    case 5:
      motorTurnVelocityRaw[5] = value;
      motorTurnVelocity255[5] = map( motorTurnVelocityRaw[5], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_5, motorTurnVelocityRaw[5] );
      break;
    case 6:
      motorTurnVelocityRaw[6] = value;
      motorTurnVelocity255[6] = map( motorTurnVelocityRaw[6], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_6, motorTurnVelocityRaw[6] );
      break;
    case 7:
      motorTurnVelocityRaw[7] = value;
      motorTurnVelocity255[7] = map( motorTurnVelocityRaw[7], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_7, motorTurnVelocityRaw[7] );
      break;
  }
}

//functions
void checkBoxToggleOff( int num ){
  setMotorNumRun(num, 0, true); //kill motor if it was on
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
  //Serial.println("set buttons on");
  for( int i = 1; i < 8; i++){
    setMotorNumRun(i, 0, true); //toggle all checkboxes on
    checkBoxToggleOff(i); //toggle all checkboxes off
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotors(){
  for( int i = 1; i < 8; i++ ){
    setMotorNumRun( i, motorTurnVelocityRaw[0] ); //set all motors to the same value
  }
  server.send(200, "text/plain", ""); //Send web page ok
}
//checkbox invidual toggles
void checkBox1Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[1] = checkState;
  if(checkState == false)
    setMotorNumRun(1, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox2Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[2] = checkState;
  if(checkState == false)
    setMotorNumRun(2, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox3Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[3] = checkState;
  if(checkState == false)
    setMotorNumRun(3, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox4Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[4] = checkState;
  if(checkState == false)
    setMotorNumRun(4, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox5Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[5] = checkState;
  if(checkState == false)
    setMotorNumRun(5, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox6Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[6] = checkState;
  if(checkState == false)
    setMotorNumRun(6, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}
void checkBox7Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[7] = checkState;
  if(checkState == false)
    setMotorNumRun(7, 0, true); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page ok
}

//motors individual velocity setters
void setMotor0(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(0, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor1(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(1, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor2(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(2, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor3(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(3, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor4(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(4, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor5(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(5, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor6(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(6, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}
void setMotor7(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNumRun(7, MRVRaw, false);
  server.send(200, "text/plain", ""); //Send web page ok
}

void runMotors(){
  for( int i = 1; i < 8; i++){
    if( checkBoxState[i] == true ){
      setMotorNumRun(i, motorTurnVelocityRaw[i], true);
    }
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void killMotors(){
  for( int i = 1; i < 8; i++){
    setMotorNumRun(i, 0, true);
  }
  server.send(200, "text/plain", ""); //Send web page
}