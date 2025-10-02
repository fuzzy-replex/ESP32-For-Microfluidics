#include <WiFi.h> // contains WiFi class which creates a WiFi object. Simplifies the connection to a WiFi network.
#include <WebServer.h> // contains WebServer class which creates a server object. Simplifies the connection to a web server.
#include "Charzard.h" // contains the HTML code and JS logic of the web server.
#include "time.h" // Contains time functions
#include "esp_sntp.h" // NTP (Network Time Protocol) client functionality
#include <vector>

/* What's next to do:
    - put dynamic motor control into manual mode.
    - put dynamic motor control into scheduling mode.
    - check for bugs and fix up code.
    - start designing scheduling queue.
    - implment dynaimcic schedluing.
    - add UI for dynaimcic scheduling.
*/

// The ssid and password are the typical wifi or network connection credentials
const char* ssid     = "ericPhone";//"YoureInAComaWakeUp"; 
const char* password = "apple123";//"WakeUpEthan";

//NTP configuration
const char* ntpServer = "pool.ntp.org"; // Standard NTP server pool
const long gmtOffSetSec = 0;      // GMT Offset for EST (-5 hours * 3600 seconds/hour)
const int daylightOffset_sec = 0;    // Daylight saving offset for EDT (+1 hour * 3600 seconds/hour)
const char* time_zone_str = "UTC0";

int currentNumOfMotors = 0; //Max number of motor in use!
const int MAX_PINS = 7; //Max number of GPIO pins available for motors.

// GPIO pins vairables
std::vector<int> motorPins; //FIX!!! Don't USE 5! 5 -> 25
const int motorPinPool[MAX_PINS] = {23, 22, 25, 4, 21, 19, 18};//possible pin order

// Motor state variables
const int MRV = 90000; //max rotational velocity
int motorTurnVelocityRaw[MAX_PINS+1] = {0}; //Raw is value from -MRV to MRV
int motorTurnVelocity255[MAX_PINS+1] = {0}; //225 is the Raw mapped to 0-255 for GPIO PWM
int motorTime[MAX_PINS] = {0}; //Scheduled time for each motor
bool checkBoxState[MAX_PINS+1] = {false}; //checkBoxState[0] never used

// Set web server object with port number
WebServer server(80);

// Global Scheduling variables
volatile uint32_t ellapseMotorTimeMS_start; //time in ms
volatile uint32_t ellapseMotorTimeMS_wait; //time in ms
int scheduledMRVRaw = 0; //scheduled motor rotational velocity
volatile uint64_t scheduledDateTimeStampUTCMS; //UTC local date and time in ms
volatile uint64_t finalDelayMsToScheduledEvent; // Will store the calculated delay from now until the scheduled event.

TaskHandle_t motorTaskHandles[MAX_PINS]; //task handle for motors
StaticTask_t motorTaskTCBs[MAX_PINS]; //task control block for motors
StackType_t motorTaskStacks[MAX_PINS][1024]; //stack for motors

typedef struct {
  int motorId; //specific motor
} MotorTaskParams_t;

MotorTaskParams_t motorTaskParams[MAX_PINS];

bool motorTaskActive[MAX_PINS] = {false}; //current task status

void setup() {
  Serial.begin(115200); // set serial baud rate to 115200 
  
  //fill motorPins vector with motor pins
  for ( int i = 0; i < currentNumOfMotors; i++) {
    motorPins.push_back(motorPinPool[i]); 
  }

  // Initialize and set GPIO
  for(int i = 0; i < currentNumOfMotors; i++){
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
  configTime(gmtOffSetSec, daylightOffset_sec, ntpServer); // Configure NTP for UTC
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
  
  /* on server http request, compare request to
     first parameter and then executes the function
     in the second parameter. This class object is 
     called by server.handleClient() in main loop. */
  // Send webpage
  server.on("/", SendWebsite); //fowardslash is sent by the website on web ip load.
  //Modify ALL motors checkbox requests
  server.on("/SET_ALL_MOTORS", setMotors); //checkbox 0 is all motors
  server.on("/UPDATE_MOTOR_ALL_CONTROL", setMotor0); //used to set all motors to a specific value
  server.on("/SET_CHECKBOXES_ON", setCheckboxesOn);
  server.on("/SET_CHECKBOXES_OFF", setCheckboxesOff);
  //Modify motors checkboxs individually requests
  server.on("/TOGGLE_CHECKBOX", toggleCheckbox); //toggle checkbox on or off
  //Modify motors velocity requests
  server.on("/SET_MRV", setMotorNum); //set a motors to a specific value
  server.on("/SET_MRV1", setMotor1);
  server.on("/SET_MRV2", setMotor2);
  server.on("/SET_MRV3", setMotor3);
  server.on("/SET_MRV4", setMotor4);
  server.on("/SET_MRV5", setMotor5);
  server.on("/SET_MRV6", setMotor6);
  server.on("/SET_MRV7", setMotor7);
  //Run motors requests
  server.on("/MANUAL_RUN", HTTP_PUT, manualMotorsRun);
  server.on("/SCHEDULE_RUN", HTTP_PUT, scheduleMotorsRun); //run motors in scheduling mode
  server.on("/KILL", killMotors);
  //Reset all data
  server.on("/RESET", reset); //reset all data
  //Scheduling
  //Schuduling Update
  server.on("/SCHEDULED_DATE_TIME", updateLocalDateTimeStampMS);
  server.on("/SCHEDULE_ELLAPSE_TIME", updateEllapseTime); 
  server.on("/SCHEDULE_MRVRaw", updateMRVRaw); 
  //Dynamic Motors
  server.on("/ADD_MOTORS", updateMotorCount); 

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
  if (num == 0) {
    // Motor 0 controls all motors.
    motorTurnVelocityRaw[0] = value;
    return; // Exit after handling the special case
  }

  int motorIndex = num - 1; // Convert 1-indexed motor ID to 0-indexed array index

  if (kill == true) {
    //void updateMotor( &stepper_driver_6, 0 ); //stop motor
    analogWrite(motorPins[motorIndex], 0); // Turn off motor
    return; // Kill takes priority, so no further processing for this call
  }

  // If not killing, update velocity and run if 'run' flag is true
  motorTurnVelocityRaw[num] = value; // Store raw value (use num for consistency with existing arrays)
  motorTurnVelocity255[num] = map(motorTurnVelocityRaw[num], -MRV, MRV, 0, 255);

  if (run == true) {
    //void updateMotor( &stepper_driver_6, motorTurnVecolityRaw[num] ); //start motor
    analogWrite(motorPins[motorIndex], motorTurnVelocity255[num]);
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
  for( int i = 1; i <= currentNumOfMotors; i++){
    checkBoxToggleOn(i); //toggle all checkboxes on
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setCheckboxesOff(){
  //Serial.println("set checkboxes off");
  for( int i = 1; i <= currentNumOfMotors; i++){
    setMotorNumRunKill(i, 0, true, true); //toggle all checkboxes off
    checkBoxToggleOff(i); //toggle all checkboxes off
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotors(){
  for( int i = 1; i <= currentNumOfMotors; i++ ){
    setMotorNumRunKill( i, motorTurnVelocityRaw[0] ); //set all motors to the same value
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

//DATETIME_INDEX=${datetimeIndex}&SUBGROUP_INDEX=${subgroupIndex}&MOTORNUM=${motorNum}&STATE=${isChecked}

//checkbox individual toggles 
void toggleCheckbox(){
  int num = server.arg("MOTORNUM").toInt(); //get the checkbox number from the request
  bool checkState = (server.arg("STATE") == "true"); //checkState saves boolean equivalance outcome.
  
  checkBoxState[num] = checkState; //update checkbox state
  if(checkState == false){
    setMotorNumRunKill(num, 0, true, true); //kill motor if it was on
  }
  
  server.send(200, "text/plain", ""); //Send web page ok
}

//motors individual velocity setters
void setMotorNum(){
  int motorNum = server.arg("MOTORNUM").toInt(); //get the motor number from the request
  int MRVRaw = server.arg("VALUE").toInt(); //return string of js int
  setMotorNumRunKill(motorNum, MRVRaw);
  server.send(200, "text/plain", ""); //Send web page ok
}

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
  for( int i = 1; i <= currentNumOfMotors; i++){
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
  
  for( int i = 0; i < currentNumOfMotors; i++){
    if(motorTaskActive[i] && checkBoxState[i+1])
      xTaskNotifyGive(motorTaskHandles[i]); //launch motor X tasks to handle time.
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

// kills all motors.
void killMotors(){
  deleteMotorTasks(); //delete all tasks
  for( int i = 1; i <= currentNumOfMotors; i++){
    setMotorNumRunKill(i, 0, true, true); //num, value, run, kill (kill takes priority)
  }
  setupMotorTasks(); //recreate all tasks
  server.send(200, "text/plain", ""); //Send web page
}

//reset all data
void reset(){
  String viewNav = server.arg("VIEW");
  for( int i = 1; i <= currentNumOfMotors; i++){
    setMotorNumRunKill(i, 0, true, true); //num, value, run, kill (kill takes priority)
  }
  setCheckboxesOff();
  server.send(200, "text/plain", "");
}

void MotorControlTask(void *pvParameters) {
  MotorTaskParams_t *params = (MotorTaskParams_t *)pvParameters;
  int motorId = params->motorId;

  while (true) {
    // Wait indefinitely until this task receives a notification to run
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // These are global scheduling variables, so they apply to all notified tasks.
    TickType_t timeUntilScheduledEvent_DT = pdMS_TO_TICKS(finalDelayMsToScheduledEvent);
    TickType_t motorRunDuration_DT = pdMS_TO_TICKS(ellapseMotorTimeMS_start);
    TickType_t motorWaitDuration_DT = pdMS_TO_TICKS(ellapseMotorTimeMS_wait);

    // Delay until the scheduled Datetime start time
    vTaskDelay(timeUntilScheduledEvent_DT); //Date delay in cpu ticks

    if (checkBoxState[motorId]) {
        // need to pass motorparam to test gpio in setMotorNumRunKill dynamically
        vTaskDelay(motorWaitDuration_DT); // Wait for ellapse before starting the motor
        setMotorNumRunKill(motorId, motorTurnVelocityRaw[motorId], true, false); // Turn motor ON
        vTaskDelay(motorRunDuration_DT); // Run ellapse for the specified duration
        setMotorNumRunKill(motorId, 0, false, true); // Turn motor OFF
    }
  }
}

//update scheduling variables
void updateLocalDateTimeStampMS(){
  String scheduledDateTimeStampMS_str = server.arg("VALUE");
  scheduledDateTimeStampUTCMS = strtoull(scheduledDateTimeStampMS_str.c_str(), NULL, 10); //convert string to unsigned long long
  server.send(200, "text/plain", ""); //Send web page ok
}
void updateEllapseTime(){
  ellapseMotorTimeMS_start = server.arg("START").toInt();
  ellapseMotorTimeMS_wait = server.arg("WAIT").toInt();

  server.send(200, "text/plain", ""); //Send web page ok
}
void updateMRVRaw(){
  scheduledMRVRaw = server.arg("VALUE").toInt();
  server.send(200, "text/plain", ""); //Send web page ok
}

// This function will initially create all currentNumOfMotors tasks.
// later refine it to only create tasks of motors that are 'added' by the user.
// For now, it makes sure all potential tasks are set up at boot.
void setupMotorTasks() {
  for (int i = 0; i < currentNumOfMotors; i++) {
    // Check if the task for this slot is not already created
    if (motorTaskHandles[i] == NULL) { // Check if handle is NULL to indicate not created
      // Initialize motor-specific parameters for this task
      motorTaskParams[i].motorId = i + 1; // Motor IDs starting from 1

      motorTaskHandles[i] = xTaskCreateStaticPinnedToCore(
        MotorControlTask,                             /* Function to implement the task */
        ("Motor" + String(i + 1) + "Task").c_str(), /* Name of the task (e.g., "Motor1Task") */
        1024,                                         /* Stack size in words (4KB) */
        &motorTaskParams[i],                              /* Task input parameter (pointer to motorParams struct) */
        1,                                            /* Priority of the task */
        motorTaskStacks[i],                           /* Stack buffer */
        &motorTaskTCBs[i],                            /* TCB buffer */
        1                                             /* Core where the task should run */
      );

      if (motorTaskHandles[i] != NULL) {
        motorTaskActive[i] = true; // Mark this task slot as active
      }
    }
  }
}

void deleteMotorTasks(){
  for (int i = 0; i < currentNumOfMotors; i++) {
    if (motorTaskActive[i] && motorTaskHandles[i] != NULL) {
      vTaskDelete(motorTaskHandles[i]);
      motorTaskHandles[i] = NULL;       // Clear the active handle
      motorTaskActive[i] = false;       // Mark handle as inactive
    }
  }
}

void resetMotor(int motornum){
  checkBoxToggleOff(motornum); //toggle checkbox off
  setMotorNumRunKill(motornum, 0, true, false); //set motor to 0)
}

void updateMotorCount() {
  int desiredNumOfMotors = server.arg("VALUE").toInt(); // Get the number of motors from the request
  int motorChange = currentNumOfMotors - desiredNumOfMotors;

  if( motorChange > 0 ) { //removing motors
    for( int i = currentNumOfMotors; i > desiredNumOfMotors; i--){
      resetMotor(i); //reset motor
      setMotorNumRunKill(i, 0, true, true); //kill motor if it was on
    }
  } else { // adding motors
    for( int i = currentNumOfMotors + 1; i < desiredNumOfMotors; i++){
      resetMotor(i); //reset motor if on
    }
  }

  currentNumOfMotors = desiredNumOfMotors; // Update the current number of motors
  updateMotorPins(currentNumOfMotors); // Update the motor count in the system
  server.send(200, "text/plain", ""); //Send web page ok
}

// resets motor pins to match dynamic motor count. This function turns all motors of when the count is changed.
void updateMotorPins(int newMotorCount) {

  motorPins.clear(); // Clear the existing vector
  for (int i = 0; i < newMotorCount; i++) {
    motorPins.push_back(motorPinPool[i]); // Fill with available pins from the pool
  }

  // Reinitialize the GPIO pins for the new motor count
  for (int i = 0; i < newMotorCount; i++) {
    pinMode(motorPins[i], OUTPUT); // Set GPIO pins to output
    digitalWrite(motorPins[i], LOW); // Set GPIO pins to low
  }
}