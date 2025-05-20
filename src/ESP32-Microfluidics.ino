#include <WiFi.h> // contains WiFi class which creates a WiFi object. Simplifies the connection to a WiFi network.
#include <WebServer.h> // contains WebServer class which creates a server object. Simplifies the connection to a web server.
#include "Charzard.h" // contains the HTML code and JS logic of the web server.
#include "time.h" // Contains time functions
#include "esp_sntp.h" // NTP (Network Time Protocol) client functionality


// The ssid and password are the typical wifi or network connection credentials
const char* ssid     = "YoureInAComaWakeUp"; 
const char* password = "WakeUpEthan";

//NTP configuration
const char* ntpServer = "pool.ntp.org"; // Standard NTP server pool
const long gmtOffset_sec = 0;      // GMT Offset for EST (-5 hours * 3600 seconds/hour)
const int daylightOffset_sec = 0;    // Daylight saving offset for EDT (+1 hour * 3600 seconds/hour)
const char* time_zone_str = "UTC0";

const int totalNumberOfMotors = 7; //number of motors

// GPIO pins vairables
const int motorPins[7] = {23, 22, 5, 4, 21, 19, 18}; //GPIO pins for motors

// Motor state variables
const int MRV = 90000; //max rotational velocity
int motorTurnVelocityRaw[8] = {0}; //Raw is value from -MRV to MRV
int motorTurnVelocity255[8] = {0}; //225 is the Raw mapped to 0-255 for GPIO PWM
int motorTime[8] = {0}; //Scheduled time for each motor
bool checkBoxState[8] = {false}; //checkBoxState[0] never used

// Set web server object with port number
WebServer server(80);

// Global Scheduling variables
volatile uint32_t ellapseMotorSwappingTimeMS; //time in ms
int scheduledMRVRaw = 0; //scheduled motor rotational velocity
volatile uint64_t scheduledDateTimeStampUTCMS; //UTC local date and time in ms
volatile uint64_t finalDelayMsToScheduledEvent; // Will store the calculated delay from now until the scheduled event.

TaskHandle_t MotorTaskKey1; //task handle for motor 1
StaticTask_t Motor1TCB; //task control block for motor 1
StackType_t Motor1Stack[1024]; //stack for motor 1

TaskHandle_t MotorTaskKey2; //task handle for motor 2
StaticTask_t Motor2TCB; //task control block for motor 2
StackType_t Motor2Stack[1024]; //stack for motor 2

TaskHandle_t MotorTaskKey3; //task handle for motor 3
StaticTask_t Motor3TCB; //task control block for motor 3
StackType_t Motor3Stack[1024]; //stack for motor 3

TaskHandle_t MotorTaskKey4; //task handle for motor 4
StaticTask_t Motor4TCB; //task control block for motor 4
StackType_t Motor4Stack[1024]; //stack for motor 4

TaskHandle_t MotorTaskKey5; //task handle for motor 5
StaticTask_t Motor5TCB; //task control block for motor 5
StackType_t Motor5Stack[1024]; //stack for motor 5

TaskHandle_t MotorTaskKey6; //task handle for motor 6
StaticTask_t Motor6TCB; //task control block for motor 6
StackType_t Motor6Stack[1024]; //stack for motor 6

TaskHandle_t MotorTaskKey7; //task handle for motor 7
StaticTask_t Motor7TCB; //task control block for motor 7
StackType_t Motor7Stack[1024]; //stack for motor 7

void setup() {
  Serial.begin(115200); // set serial baud rate to 115200 
  
  // Initialize and set GPIO
  for(int i = 0; i < 7; i++){
    pinMode(motorPins[i], OUTPUT); //set GPIO pins to output
    digitalWrite(motorPins[i], LOW); //set GPIO pins to low
  }

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


  // Initialize NTP client
  Serial.println("Configuring time with NTP server (UTC)...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); // Configure NTP for UTC
  setenv("TZ", time_zone_str, 1); // Set the timezone environment variable to UTC
  tzset(); // Apply the timezone

  // Optional: Print current synchronized time (in UTC) to verify
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){ // getLocalTime will now return UTC time because TZ is UTC0
      Serial.println("Failed to obtain time from NTP server (UTC).");
  } else {
      Serial.println("Time synchronized successfully (UTC)!");
      Serial.printf("Current synchronized UTC time: %s", asctime(&timeinfo));
  }
  
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
  server.on("/MANUAL_RUN", manualMotorsRun);
  server.on("/SCHEDULE_RUN", scheduleMotorsRun); //run motors in scheduling mode
  server.on("/KILL", killMotors);
  //Reset all data
  server.on("/RESET", reset); //reset all data
  //Scheduling
  //Schuduling Update
  server.on("/SCHEDULED_DATE_TIME", updateLocalDateTimeStampMS);
  server.on("/SCHEDULE_ELLAPSE_TIME", updateEllapseTime); 
  server.on("/SCHEDULE_MRVRaw", updateMRVRaw); 

  //Start Server
  server.begin();


  // Utalizing ESP32's FreeRTOS to create a tasks to run time sensative code.
  /* Size allocated from 7 tasts = 4 kb * 7 = 28 kb 
  of stack memory out of 320kb DRAM and 200kb IRAM.*/ 
  setupMotorTasks();
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
        analogWrite(motorPins[0], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[1] = value;
      motorTurnVelocity255[1] = map( motorTurnVelocityRaw[1], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_1, motorTurnVelocityRaw[1] );
        analogWrite(motorPins[0], motorTurnVelocity255[1]);
      }
      break;
    case 2:
      if(kill == true){
        //void updateMotor( &stepper_driver_2, 0 );
        analogWrite(motorPins[1], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[2] = value;
      motorTurnVelocity255[2] = map( motorTurnVelocityRaw[2], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_2, motorTurnVelocityRaw[2] );
        analogWrite(motorPins[1], motorTurnVelocity255[2]);
      }
      break;
    case 3:
      if(kill == true){
        //void updateMotor( &stepper_driver_3, 0 );
        analogWrite(motorPins[2], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[3] = value;
      motorTurnVelocity255[3] = map( motorTurnVelocityRaw[3], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_3, motorTurnVelocityRaw[3] );
        analogWrite(motorPins[2], motorTurnVelocity255[3]);
      }
      break;
    case 4:
      if(kill == true){
        //void updateMotor( &stepper_driver_4, 0 );
        analogWrite(motorPins[3], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[4] = value;
      motorTurnVelocity255[4] = map( motorTurnVelocityRaw[4], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_4, motorTurnVelocityRaw[4] );
        analogWrite(motorPins[3], motorTurnVelocity255[4]);
      }
      break;
    case 5:
      if(kill == true){
        //void updateMotor( &stepper_driver_5, 0 );
        analogWrite(motorPins[4], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[5] = value;
      motorTurnVelocity255[5] = map( motorTurnVelocityRaw[5], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_5, motorTurnVelocityRaw[5] );
        analogWrite(motorPins[4], motorTurnVelocity255[5]);
      }
      break;
    case 6:
      if(kill == true){
        //void updateMotor( &stepper_driver_6, 0 );
        analogWrite(motorPins[5], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[6] = value;
      motorTurnVelocity255[6] = map( motorTurnVelocityRaw[6], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_6, motorTurnVelocityRaw[6] );
        analogWrite(motorPins[5], motorTurnVelocity255[6]);
      }
      break;
    case 7:
      if(kill == true){
        //void updateMotor( &stepper_driver_7, 0 );
        analogWrite(motorPins[6], 0);
        break;
      }// else kill == false
      motorTurnVelocityRaw[7] = value;
      motorTurnVelocity255[7] = map( motorTurnVelocityRaw[7], -MRV, MRV, 0, 255 );
      if(run == true){
        //void updateMotor( &stepper_driver_7, motorTurnVelocityRaw[7] );
        analogWrite(motorPins[6], motorTurnVelocity255[7]);
      }
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

//run motors
////run motors in manual mode
void manualMotorsRun(){
  Serial.printf("Manual Mode\n");
  for( int i = 1; i < 8; i++){
    if( checkBoxState[i] == true ){
      setMotorNumRunKill(i, motorTurnVelocityRaw[i], true);
    }
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

////run motors in schedule mode
void scheduleMotorsRun(){
  Serial.printf("Scheduling Mode\n");

  // schedule calculations outside of tasks
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL); // Get the current time
  uint64_t current_unix_ms  = (tv_now.tv_sec * 1000) + (tv_now.tv_usec / 1000); // Convert to milliseconds
  finalDelayMsToScheduledEvent = scheduledDateTimeStampUTCMS - current_unix_ms; //time until scheduled date ms
  
  for( int i = 1; i < 8; i++){
    scheduleMotors(i); //Run all scheduled motors
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

// kills all motors.
void killMotors(){
  deleteMotorTasks(); //delete all tasks
  for( int i = 1; i < 8; i++){
    setMotorNumRunKill(i, 0, true, true); //num, value, run, kill (kill takes priority)
  }
  setupMotorTasks(); //recreate all tasks
  server.send(200, "text/plain", ""); //Send web page
}

//reset all data
void reset(){
  String viewNav = server.arg("plain"); //plain means send raw data (viewState)
  for( int i = 0; i < 8; i++){
    setMotorNumRunKill(i, 0, true, true); //num, value, run, kill (kill takes priority)
    setMotorNumRunKill(i, 0); //setting motors to 0
  }
  if(viewNav == "Scheduling Mode"){
    setCheckboxesOn(); //turn On all checkboxes in background of Scheduling view
    //!check later becuase setting checkbox in background might not be nessasary
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void scheduleMotors(int num){
  switch (num)
  {
    case 1:
      xTaskNotifyGive( MotorTaskKey1 ); //notify task to run in parallel
      break;
    case 2:
      xTaskNotifyGive( MotorTaskKey2 );
      break;
    case 3:
      xTaskNotifyGive( MotorTaskKey3 );
      break;
    case 4:
      xTaskNotifyGive( MotorTaskKey4 );
      break;
    case 5:
      xTaskNotifyGive( MotorTaskKey5 );
      break;
    case 6:
      xTaskNotifyGive( MotorTaskKey6 );
      break;
    case 7:
      xTaskNotifyGive( MotorTaskKey7 );
      break;
    default:
      break;
  }
}

//update scheduling variables
void updateLocalDateTimeStampMS(){
  String valueStr = server.arg("VALUE");
  scheduledDateTimeStampUTCMS = strtoull(valueStr.c_str(), NULL, 10); //convert string to unsigned long long
  server.send(200, "text/plain", ""); //Send web page ok
}
void updateEllapseTime(){
  ellapseMotorSwappingTimeMS = server.arg("VALUE").toInt();
  server.send(200, "text/plain", ""); //Send web page ok
}
void updateMRVRaw(){
  scheduledMRVRaw = server.arg("VALUE").toInt();
  server.send(200, "text/plain", ""); //Send web page ok
}

void Motor1Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS*6 );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    setMotorNumRunKill(1, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT); //delay for ellapseMotorSwappingTimeMS
    setMotorNumRunKill(1, 0, false, true);
  }
}

void Motor2Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS*6 );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    setMotorNumRunKill(2, scheduledMRVRaw, true, false); //on
    delay(ellapseMotorSwappingTimeMS*1);
    setMotorNumRunKill(2, 0, false, true);
  }
}

void Motor3Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(3, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(3, 0, false, true);
  }
}

void Motor4Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*2);
    setMotorNumRunKill(4, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(4, 0, false, true);
  }
}

void Motor5Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*3);
    setMotorNumRunKill(5, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(5, 0, false, true);
  }
}

void Motor6Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*4);
    setMotorNumRunKill(6, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(6, 0, false, true);
  }
}

void Motor7Task(void *pvParameters) {
  while(true){
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );/*pdTRUE = set binary semaphore count on notifictaion to 0,
                                                portMAX_DELAY = wait indefinitely until binary semaphore given.
    */
    TickType_t timeUntilDate_DT = pdMS_TO_TICKS( finalDelayMsToScheduledEvent );
    TickType_t ellapseMotorSwappingTimeMS_DT = pdMS_TO_TICKS( ellapseMotorSwappingTimeMS );
    vTaskDelay(timeUntilDate_DT); //delay for timeUntilDateMS
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*5);
    setMotorNumRunKill(7, scheduledMRVRaw, true, false); //on
    vTaskDelay(ellapseMotorSwappingTimeMS_DT*1);
    setMotorNumRunKill(7, 0, false, true);
  }
}

void setupMotorTasks(){
  if(MotorTaskKey1 == NULL) {
    MotorTaskKey1 = xTaskCreateStaticPinnedToCore(
      Motor1Task, /* Function to implement the task */
      "Motor1Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor1Stack, /* Stack handle. */
      &Motor1TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey2 == NULL) {
    MotorTaskKey2 = xTaskCreateStaticPinnedToCore(
      Motor2Task, /* Function to implement the task */
      "Motor2Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor2Stack, /* Stack handle. */
      &Motor2TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey3 == NULL) {
    MotorTaskKey3 = xTaskCreateStaticPinnedToCore(
      Motor3Task, /* Function to implement the task */
      "Motor3Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor3Stack, /* Stack handle. */
      &Motor3TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey4 == NULL) {
    MotorTaskKey4 = xTaskCreateStaticPinnedToCore(
      Motor4Task, /* Function to implement the task */
      "Motor4Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor4Stack, /* Stack handle. */
      &Motor4TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey5 == NULL) {
    MotorTaskKey5 = xTaskCreateStaticPinnedToCore(
      Motor5Task, /* Function to implement the task */
      "Motor5Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor5Stack, /* Stack handle. */
      &Motor5TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey6 == NULL) {
    MotorTaskKey6 = xTaskCreateStaticPinnedToCore(
      Motor6Task, /* Function to implement the task */
      "Motor6Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor6Stack, /* Stack handle. */
      &Motor6TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
  if(MotorTaskKey7 == NULL) {
    MotorTaskKey7 = xTaskCreateStaticPinnedToCore(
      Motor7Task, /* Function to implement the task */
      "Motor7Task", /* Name of the task */
      1024, /* stack allocated in words. 1024 words = 4096 bytes */
      NULL, /* Task input parameter */
      1, /* Priority of the task */
      Motor7Stack, /* Stack handle. */
      &Motor7TCB, /* TCB buffer */
      1); /* Core where the task should run */
  }
}

void deleteMotorTasks(){
  if (MotorTaskKey1 != NULL) {
    vTaskDelete(MotorTaskKey1);
    MotorTaskKey1 = NULL; // Clear handle after deletion
  }
  if (MotorTaskKey2 != NULL) {
    vTaskDelete(MotorTaskKey2);
    MotorTaskKey2 = NULL;
  }
  if (MotorTaskKey3 != NULL) {
    vTaskDelete(MotorTaskKey3);
    MotorTaskKey3 = NULL;
  }
  if (MotorTaskKey4 != NULL) {
    vTaskDelete(MotorTaskKey4);
    MotorTaskKey4 = NULL;
  }
  if (MotorTaskKey5 != NULL) {
    vTaskDelete(MotorTaskKey5);
    MotorTaskKey5 = NULL;
  }
  if (MotorTaskKey6 != NULL) {
    vTaskDelete(MotorTaskKey6);
    MotorTaskKey6 = NULL;
  }
  if (MotorTaskKey7 != NULL) {
    vTaskDelete(MotorTaskKey7);
    MotorTaskKey7 = NULL;
  }
}