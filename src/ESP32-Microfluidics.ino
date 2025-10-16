#include <WiFi.h> // contains WiFi class which creates a WiFi object. Simplifies the connection to a WiFi network.
#include <WebServer.h> // contains WebServer class which creates a server object. Simplifies the connection to a web server.
#include "Charzard.h" // contains the HTML code and JS logic of the web server.
#include <time.h> // Contains time functions
#include <esp_sntp.h> // NTP (Network Time Protocol) client functionality
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
std::vector<std::vector<int>> motorTurnVelocityRaw; //Raw is value from -MRV to MRV
std::vector<std::vector<int>> motorTurnVelocity255; //225 is the Raw mapped to 0-255 for GPIO PWM
int motorTime[MAX_PINS] = {0}; //Scheduled time for each motor
bool checkBoxState[MAX_PINS+1] = {false}; //checkBoxState[0] never used

// Set web server object with port number
WebServer server(80);

// Global Scheduling variables
std::vector<volatile uint32_t> elapseMotorTimeMS; //time in ms
int scheduledMRVRaw = 0; //scheduled motor rotational velocity
volatile uint64_t scheduledDateTimeStampUTCMS; //UTC local date and time in ms
volatile uint64_t finalDelayMsToScheduledEvent; // Will store the calculated delay from now until the scheduled event.

TaskHandle_t motorTaskHandles[MAX_PINS]; //task handle for motors
StaticTask_t motorTaskTCBs[MAX_PINS]; //task control block for motors
StackType_t motorTaskStacks[MAX_PINS][1024]; //stack for motors
int subgroupCount; // Track number of motor subgroups in scheduling mode

typedef struct {
  int motorId; //specific motor
} MotorTaskParams_t;

MotorTaskParams_t motorTaskParams[MAX_PINS];

bool motorTaskActive[MAX_PINS] = {false}; //current task status

//Smal refactor
int currentSubGroup = 0; //index of subgroup currently running
std::vector<std::vector<bool>> checkBoxStateSubgroups; // [subgroup][motor]
std::vector<std::vector<int>> motorTurnVelocityRawSubgroups; // [subgroup][motor]
std::vector<uint32_t> elapseMotorTimeMSSubgroups;


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

  // Initialize motor state vectors
  subgroupCount = 1; //at least one subgroup

  elapseMotorTimeMSSubgroups.resize(subgroupCount); //indexed at 1
  checkBoxStateSubgroups.resize(subgroupCount);
  motorTurnVelocityRawSubgroups.resize(subgroupCount);
  //indexed at 0:
  elapseMotorTimeMSSubgroups[0] = 0;
  //indexed at 1:
  checkBoxStateSubgroups[0].resize(currentNumOfMotors+1); //first index not used
  motorTurnVelocityRawSubgroups[0].resize(currentNumOfMotors+1); //first index special

  // WIFI CONNECTION SETUP
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
  server.on("/SET_ALL_MOTORS", setMotors); // Depends on setMotor0(), assigns manual motors to allMotorValue MRVRAW[0]
  server.on("/UPDATE_MOTOR_ALL_CONTROL", setMotor0); //updates first index of MRV which is used to set all motors (manual)
  server.on("/SET_CHECKBOXES_ON", setCheckboxesOn);
  server.on("/SET_CHECKBOXES_OFF", setCheckboxesOff);
  //Modify motors checkboxs individually requests
  server.on("/TOGGLE_CHECKBOX", toggleCheckbox); //toggle checkbox on or off
  //Modify motors velocity requests
  server.on("/SET_MRV", setMotorNum); //set a motors to a specific value
  //Run motors requests
  server.on("/MANUAL_RUN", HTTP_PUT, manualMotorsRun);
  server.on("/SCHEDULE_RUN", HTTP_PUT, scheduleMotorsRun); //run motors in scheduling mode
  server.on("/KILL", killMotors);
  //Reset all data
  server.on("/RESET", reset); //reset all data
  //Scheduling
  //Schuduling Update
  server.on("/SCHEDULED_DATE_TIME", updateLocalDateTimeStampMS);
  server.on("/UPDATE_ELLAPSE_TIME", updateElapsetime);
  //Dynamic Motors
  server.on("/ADD_MOTORS", updateMotorCount); 
  //Dynamic Elapse Time Fields
  server.on("/ADD_ELAPSE_FIELD", addMotorElapseGroups);
  server.on("/REMOVE_ELAPSE_FIELD", removeMotorElapseGroups);

  //Start Server
  server.begin();


  // Utilizing ESP32's FreeRTOS to create a tasks to run time sensitive code.
  /* Size allocated from 8 tasks = 4 kb * 8 = 32 kb
  of stack memory out of 320kb DRAM and 200kb IRAM. */
  setupTasksController(); //this task will control all motor tasks.
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
              regardless of value.
    manual:   If true, function is called in manual mode.
    */

void setMotorNumRunKill( int num, int value, int run = false, int kill = false, int subGroupIndex = -1 ){
  int sg = currentSubGroup; //save current subgroup
  if (subGroupIndex != -1)
  {
    sg = subGroupIndex;
  }
  

  if (num == 0) {
    // Motor 0 controls all motors.
    motorTurnVelocityRaw[sg][num] = value; // Update the raw value for motor 0
    return; // Exit after handling the special case
  }

  int motorIndex = num - 1; // Convert 1-indexed motor ID to 0-indexed array index

  if (kill == true) {
    analogWrite(motorPins[motorIndex], 0); // Turn off motor
    //updateMotor( num, motorTurnVelocityRaw[num][0] ); // Update motor to 0 velocity
    return; // Kill takes priority, so no further processing for this call
  }

  // If not killing, update velocity and run if 'run' flag is true
  motorTurnVelocityRaw[sg][num] = value; // Store raw value (use num for consistency with existing arrays)
  int mapped = map(motorTurnVelocityRaw[sg][num], -MRV, MRV, 0, 255);

  if (run == true) {
    //updateMotor( num, motorTurnVelocity[num] ); // Update motor using the new velocity
    analogWrite(motorPins[motorIndex], mapped);
  }
}

//manual checkbox toggles
void checkBoxToggleOff( int num){
  int sg = 0;
  setMotorNumRunKill(num, 0, true, true, sg); //kill motor if it was on
  //toggle checkbox to off
  if( checkBoxStateSubgroups[sg][num] == true )
    checkBoxStateSubgroups[sg][num] = false;
}

void checkBoxToggleOn( int num){
  int sg = 0; //save current subgroup
  if( checkBoxStateSubgroups[sg][num] == false )
    checkBoxStateSubgroups[sg][num] = true;
}

void setCheckboxesOn(){
  for( int i = 1; i <= currentNumOfMotors; i++){
    checkBoxToggleOn(i); //toggle all checkboxes on
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

void setCheckboxesOff(){
  for( int i = 1; i <= currentNumOfMotors; i++){
    checkBoxToggleOff(i); //toggle all checkboxes off
  }
  server.send(200, "text/plain", ""); //Send web page ok
}
//end manual checkbox toggles

void setMotors(){ //for manual mode only - first subgroup
  int sg = 0;
  for( int i = 1; i <= currentNumOfMotors; i++ ){
    setMotorNumRunKill( i, motorTurnVelocityRaw[sg][0], false, false, sg ); //set all motors to the same value
  }
  server.send(200, "text/plain", ""); //Send web page ok
}

//DATETIME_INDEX=${datetimeIndex}&SUBGROUP_INDEX=${subgroupIndex}&MOTORNUM=${motorNum}&STATE=${isChecked}

//checkbox individual toggles (manual & scheduling mode)
void toggleCheckbox(){
  int subgroupIndex = server.arg("SUBGROUP_INDEX").toInt();
  int num = server.arg("MOTORNUM").toInt(); //get the checkbox number from the request
  bool checkState = (server.arg("STATE") == "true"); //checkState saves boolean equivalance outcome.
  
  checkBoxStateSubgroups[subgroupIndex][num] = checkState; //update checkbox state
  if(checkState == false){
    setMotorNumRunKill(num, 0, true, true, subgroupIndex); //kill motor if it was on
  }
  
  server.send(200, "text/plain", ""); //Send web page ok
}

//motors individual velocity setters
void setMotorNum(){
  int motorNum = server.arg("MOTORNUM").toInt(); //get the motor number from the request
  int MRVRaw = server.arg("VALUE").toInt(); //return string of js int
  int sg = server.arg("SUBGROUP_INDEX").toInt();
  setMotorNumRunKill(motorNum, MRVRaw, false, false, sg);

  server.send(200, "text/plain", ""); //Send web page ok
}

void setMotor0(){
  int MRVRaw = server.arg("VALUE").toInt(); //return string of js int
  int sg = 0; //manual mode only first subgroup
  setMotorNumRunKill(0, MRVRaw, false, false, sg);
  server.send(200, "text/plain", ""); //Send web page ok
}

//run motors
////run motors in manual mode
void manualMotorsRun(){
  Serial.printf("Manual Mode\n");
  int sg = 0; //manual mode only first subgroup
  for( int i = 1; i <= currentNumOfMotors; i++){
    if( checkBoxState[i] == true ){
      setMotorNumRunKill(i, motorTurnVelocityRaw[sg][i], true);
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

  xTaskNotifyGive(motorTaskHandles[0]); //notify controller task to run all motors
  server.send(200, "text/plain", ""); //Send web page ok
}

//IMPORTANT PUT TASKSCONTROLTASKS INTO RESET FUNCTINO SO ON KILL IT KILLS THE CONTROL TASK
void TasksControlTasks(void *pvParameters) {
  MotorTaskParams_t *params = (MotorTaskParams_t *)pvParameters;
  int motorId = params->motorId;

  while (true) {
    // Wait indefinitely until this task receives a notification to run
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(finalDelayMsToScheduledEvent)); 

    //replace true with vect of motors to run !empty
    for(int sg = 0; sg < subgroupCount; sg++){ //loop through all subgroups
      currentSubGroup = sg;

      //loop through all motors and notify active ones with checkbox on
      for( int j = 1; j <= currentNumOfMotors; j++){
        if(checkBoxStateSubgroups[sg][j])
          xTaskNotifyGive(motorTaskHandles[j]);
      }

      // Wait until group of motors are done before starting the next.
      bool allMotorsDone = false;
      while(!allMotorsDone){
        allMotorsDone = true;
        for(int j = 1; j <= currentNumOfMotors; j++){
          if(motorTaskActive[j]){
            allMotorsDone = false;
            break;
          }
        }
      }
    }
  }
}

void MotorControlTask(void *pvParameters) {
  MotorTaskParams_t *params = (MotorTaskParams_t *)pvParameters;
  int motorId = params->motorId;

  while (true) {
    // Wait indefinitely until this task receives a notification to run
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    int sg = currentSubGroup; //capture subgroup at notification time
    TickType_t motorRunDuration_DT = pdMS_TO_TICKS(elapseMotorTimeMSSubgroups[sg]); //convert ms to ticks

    if (checkBoxStateSubgroups[sg][motorId]) {
        motorTaskActive[motorId] = true; // Mark this task as active

        int mrv = motorTurnVelocityRawSubgroups[sg][motorId];
        setMotorNumRunKill(motorId, mrv, true, false); // Turn motor ON
        vTaskDelay(motorRunDuration_DT); // Run ellapse for the specified duration
        setMotorNumRunKill(motorId, 0, false, true); // Turn motor OFF
    }
    motorTaskActive[motorId] = false; // Mark this task as inactive
  }
}


// Thread used to control all threads
void setupTasksController() {
  int i = 0;
  // Check if the task for this slot is not already created
  if (motorTaskHandles[i] == NULL) { // Check if handle is NULL to indicate not created
    // Initialize motor-specific parameters for this task
    motorTaskParams[i].motorId = i; // Motor IDs starting from 1

    motorTaskHandles[i] = xTaskCreateStaticPinnedToCore(
      TasksControlTasks,                             /* Function to implement the task */
      ("Motor" + String(i) + "Task").c_str(), /* Name of the task (e.g., "Motor0Task") */
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

// Motor threads which vary based on number of motors allocated.
void setupMotorTasks() {
  for (int i = 1; i <= currentNumOfMotors; i++) {
    // Check if the task for this slot is not already created
    if (motorTaskHandles[i] == NULL) { // Check if handle is NULL to indicate not created
      // Initialize motor-specific parameters for this task
      motorTaskParams[i].motorId = i; // Motor IDs starting from 1

      motorTaskHandles[i] = xTaskCreateStaticPinnedToCore(
        MotorControlTask,                             /* Function to implement the task */
        ("Motor" + String(i) + "Task").c_str(), /* Name of the task (e.g., "Motor1Task") */
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

//update scheduling variables
void updateLocalDateTimeStampMS(){
  String scheduledDateTimeStampMS_str = server.arg("VALUE");
  scheduledDateTimeStampUTCMS = strtoull(scheduledDateTimeStampMS_str.c_str(), NULL, 10); //convert string to unsigned long long
  server.send(200, "text/plain", ""); //Send web page ok
}

void updateElapsetime(){
  int sg = server.arg("SUBGROUP_INDEX").toInt();
  int elapseTimeMS = server.arg("ELAPSE_TIME").toInt();

  elapseMotorTimeMSSubgroups[sg-1] = elapseTimeMS; //update elapse time for subgroup

  server.send(200, "text/plain", ""); //Send web page ok
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

  // size each subgroup vector to hold motor indexes up to desiredNumOfMotors
  for(int sg = 0; sg < subgroupCount; sg++){
    checkBoxStateSubgroups[sg].resize(desiredNumOfMotors + 1, false); // +1 to account for 1-based indexing
    motorTurnVelocityRawSubgroups[sg].resize(desiredNumOfMotors + 1, 0); // +1 to account for 1-based indexing
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

// Allocates by 1
void addMotorElapseGroups(){
  subgroupCount = server.arg("SG_COUNT").toInt();  
  elapseMotorTimeMS.push_back(0); // Add a new elapse time entry initialized to 0
  checkBoxStateSubgroups.push_back(std::vector<bool>(currentNumOfMotors+1, false)); // Add a new checkbox state vector initialized to false
  motorTurnVelocityRawSubgroups.push_back(std::vector<int>(currentNumOfMotors+1, 0)); // Add a new motor turn velocity vector initialized to 0
  server.send(200, "text/plain", ""); //Send web page ok
}

// Deallocates by 1
void removeMotorElapseGroups(){
  subgroupCount = server.arg("SG_COUNT").toInt();
  elapseMotorTimeMS.pop_back(); // Remove the last elapse time entry
  checkBoxStateSubgroups.pop_back(); // Remove the last checkbox state vector
  motorTurnVelocityRawSubgroups.pop_back(); // Remove the last motor turn velocity vector
  server.send(200, "text/plain", ""); //Send web page ok
}