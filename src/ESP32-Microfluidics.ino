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
  //Modify ALL motors checkbox requests
  server.on("/SET_ALL_MOTORS", setMotors); //checkbox 0 is all motors
  server.on("/UPDATE_MOTOR_ALL_CONTROL", setMotor0); //used in set_all_motors to set all motors to this value
  server.on("/SET_BUTTONS_ON", setButtonsOn);
  server.on("/SET_BUTTONS_OFF", setButtonsOff);
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
  server.handleClient(); //handle client requests
}

// Web driving related functions
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


//functions
bool checkBoxToggleOff( int num ){
  setMotorNum(num, 0); //kill motor if it was on
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
  for( int i = 1; i < 8; i++ ){
    setMotorNum( i, motorTurnVelocityRaw[0] ); //set all motors to the same value
  }
  server.send(200, "text/plain", ""); //Send web page
}
//checkbox invidual toggles
void checkBox1Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[1] = checkState;
  if(checkState == false)
    setMotorNum(1, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox2Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[2] = checkState;
  if(checkState == false)
    setMotorNum(2, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox3Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[3] = checkState;
  if(checkState == false)
    setMotorNum(3, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox4Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[4] = checkState;
  if(checkState == false)
    setMotorNum(4, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox5Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[5] = checkState;
  if(checkState == false)
    setMotorNum(5, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox6Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[6] = checkState;
  if(checkState == false)
    setMotorNum(6, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
void checkBox7Toggle(){
  int checkState = server.arg("STATE").toInt();
  checkBoxState[7] = checkState;
  if(checkState == false)
    setMotorNum(7, 0); //kill motor if it was on
  server.send(200, "text/plain", ""); //Send web page
}
//motors individual velocity setters
void setMotor0(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(0, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor1(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(1, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor2(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(2, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor3(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(3, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor4(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(4, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor5(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(5, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor6(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(6, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}
void setMotor7(){
  int MRVRaw = server.arg("VALUE").toInt();
  setMotorNum(7, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page
}


void runMotors(){
  for( int i = 1; i < 8; i++){
    if( checkBoxState[i] == true ){
      setMotorNum(i, motorTurnVelocityRaw[i]); //Void updateMotor( &stepper_driver_0, motorTurnVelocityRaw );
    }
  }
  server.send(200, "text/plain", ""); //Send web page
}

void killMotors(){
  for( int i = 1; i < 8; i++){
    checkBoxToggleOff(i); //toggle all checkboxes off.
    setMotorNum(i, 0); //kill motor if didn't kill within checkBoxToggleOff
  }
  server.send(200, "text/plain", ""); //Send web page
}

void setMotorNum( int num, int value ){
  switch (num)
  {
    case 0:
      motorTurnVelocityRaw[0] = value;
      motorTurnVelocity255[0] = map( motorTurnVelocityRaw[0], 0, MRV, 0, 255 );
      break;
    case 1:
      motorTurnVelocityRaw[1] = value;
      motorTurnVelocity255[1] = map( motorTurnVelocityRaw[1], 0, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_1, motorTurnVelocityRaw[1] );
      analogWrite(output22, motorTurnVelocity255[1]);
      break;
    case 2:
      motorTurnVelocityRaw[2] = value;
      motorTurnVelocity255[2] = map( motorTurnVelocityRaw[2], -MRV, MRV, 0, 255 );
      //void updateMotor( &stepper_driver_2, motorTurnVelocityRaw[2] );
      analogWrite(output23, motorTurnVelocity255[2]);
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
  //no 200 ok response needed for this function
}