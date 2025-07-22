const char PAGE_MAIN[] PROGMEM = R"rawliteral(
<!-- listening for PAGE_MAIN server connection -->
<!-- PROGMEM specifies that PAGE_MAIN is stored in flash memory to conserve space in RAM. -->
<!-- R"rawliteral means store the array as written for formatting embedded HTML -->
<!-- html section -->
<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
    <head>
        <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
        html {
            font-family: "Verdana", "Arial", sans-serif;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        .button {
            background-color: #195B6A;
            border: none;
            color: white;
            padding: 16px;
            text-decoration: none;
            font-size: 30px;
            margin: 2px;
            cursor: pointer;
        }
        .title {
            font-family: "Verdana", "Arial", sans-serif;
            font-weight: bold;
            font-size: 24px;
            line-height: 25px;
            padding: 10px 5px;
            color: #ffffff;
        }
        .wrapper {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            align-items: center;
            margin: 0 auto;
        }
        .myDiv {
            border: 4px outset lightblue;
        }
        .topnav {
            display: flex;
            flex-wrap: nowrap; /* Don't stack */
            background-color: #222; /*background of navbar*/
            padding: 10px;
            align-items: center;
            justify-content: space-between;
            gap: 15px;
        }
        .topnav a {
            color: #f2f2f2;
            text-align: center;
            padding: 14px 16px;
            text-decoration: none;
            white-space: nowrap; /* Prevents link text from breaking */
        }
        .topnav a:hover {
            background-color: #ddd;
            color: black;
        }
        .nav-links {
            display: flex;
            flex-wrap: nowrap;
            gap: 10px;
            flex-shrink: 0; /* keep links from shrinking */
        }
        .topnavTitle {
            font-weight: bold;
            color: white;
            flex: 1 1 auto;
            min-width: 10ch;       /* Don't shrink smaller than 10 characters wide */
            max-width: 100%;       /* Let it grow as needed */
            word-break: break-word;
            text-align: left;
            margin: auto;
        }
        table {
            font-family: arial, sans-serif;
            border-collapse: collapse;
            max-width: 1070px;
            table-layout: fixed; /*allow table to grow with divs*/
            margin-top: 15px;
            margin-left: auto;
            margin-right: auto;
        }
        td, th {
            border: 1px solid #b7b7b7;
            text-align: center;
            padding: 8px;
            color: white
        }
        th.datetime-col, td.datetime-col {
            width: 170px;
        }
        th.velocity-col, td.velocity-col {
            min-width: 100px;
            max-width: 420px;
            width: auto;
        }
        th.ellapse-col, td.ellapse-col {
            width: 200px;
        }
        tr:nth-child(even) {
            background-color: #434343;
        }
        .velocity-input {
            width: 70px;
        }
        .crumpleByWord {
            color: white;
            flex: 1 1 auto;
            min-width: 10ch;       /* Don't shrink smaller than 10 characters wide */
            max-width: 100%;       /* Let it grow as needed */
            word-break: break-word;
            text-align: center;
        }
        </style>
    </head>
            <!-- Background color of Body!!! -->
    <body style="Background-color: #383838">
        
<!-- NavBar Section! -->
        <div class="topnav">
            <span 
                class="topnavTitle">Microfluidics Pump Controller
            </span>
            <div> <!-- Dynamically change motor num numbox -->
                <input type="number" min="0" max="30" value="0" id="numMotors" onchange="addMotors(this.value)">Change Num Motors</input>
            </div>
            <div class="nav-links">
                <a href="javascript:void(0)" onclick="showView('Manual Mode')">Manual Mode</a>
                <a href="javascript:void(0)" onclick="showView('Scheduling Mode')">Scheduling Mode</a>
                <!-- javascript:void(0) does nothing. This allows onclick to call a js function without redirection -->
            </div>
        </div>
        
<!-- Manual Mode Section! -->
        <div id="manualMode" style="display: block;">
            <h2 class="title">Manual Mode</h2>
            <p style="color: rgb(255, 255, 255);">Motor Rotational Velocity:</p>
            <p style="color: rgb(255, 255, 255); font-size: 10px;">Accepted Range [-90000, 90000]</p>
        
            <!-- Modify All MVR and All Checkboxes GUI-->
            <div class="wrapper">
                <div class="myDiv">
                    <p ><button onclick="setAllMotors()">Set All Motors</button></p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocityAll" onchange="updateAllMRVControl(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p><button onclick="setCheckboxesOn()">Set Checkboxes On</button></p>
                    <p><button onclick="setCheckboxesOff()">Set Checkboxes Off</button></p>
                </div>
            </div>

<!-- Dynamic Testing Section! --> <!-- This section is for testing dynamic content addition --> <!-- -->
            
            <button onclick="addMotor()">Add Motors</button>
            <button onclick="removeMotor()">Remove Motors</button>
<!-- End Dynamic Testing Section! -->

            <!-- Motors 1 though 7 GUI-->
            <div class="wrapper" id="motorControlsManual">
                <!-- dynamically allocated using js -->
            </div>

            <!-- run and kill buttons moved outside of page div sections -->
        </div>


<!-- Schedule Mode Section! -->
        <div id="schedulingMode" style="display: none;">
            <h2 class="title">Scheduling Mode</h2>
            <table>
                <!-- Titles -->
                <tr>
                    <th 
                        class="datetime-col">DateTime
                    </th>

                    <th class="velocity-col">
                        <div>
                            MRV <small><small></small>(<sup>&mu;_steps</sup>&frasl;<sub>sec</sub><small></small>)
                        </div>
                        <div style="font-size: 0.6em;">
                            [-90000, 90000]
                        </div>
                    </th>

                    <th>
                        <div class="ellapse-col crumpleByWord">
                            Elapsed Time
                        </div>
                        <div style="font-size: 0.6em;">
                            (Motor on durration)
                        </div>
                    </th>
                </tr>
                <!-- Datetime Row -->
                <tr>
                    <td class="datetime-col">
                        <input id="dateTime" type="datetime-local" style="width: 170px;">
                    </td>
                    <td class="velocity-col">
                        <!-- Motors 1 though 7 GUI-->
                        <div class="wrapper" id="motorControlsSchedule">
                            <!-- dynamically allocated using js -->
                        </div>
                    </td>
                    <td class="ellapse-col">
                        <div style="display: flex; align-items: center; justify-content: center;">
                            <input style="width:40px;" id="ellapseTimeMotorSwitchingHours" type="number" value="0" min="0" onchange="checkEllapseTime()"> hrs&nbsp;:&nbsp;
                            <input style="width:40px;" id="ellapseTimeMotorSwitchingMinutes" type="number" value="0" min="0" onchange="checkEllapseTime()"> mins&nbsp;:&nbsp;
                            <input style="width:40px;" id="ellapseTimeMotorSwitchingSeconds" type="number" value="0" min="0" onchange="checkEllapseTime()"> secs
                        </div>
                        <div id="ellapseTimeWarning" style="color: orange; display: none; font-size: 0.8em;">
                            Range (0,inf).
                        </div>
                    </td>
                    <!-- Dynamically Add new Motor Modify and ElapseTime -->
                    <!-- End Dynamically Add new Motor Modify and ElapseTime -->
                </tr>
                <!-- Dynamic Add New Datetime Row Here -->
            </table>
        </div>
        <!-- outside both page div sections. The following code is global to all pages on website -->
        <!-- Run and kill buttons -->
        <div class="wrapper" style="margin-top: 10px;">
            <button class="button" onclick="run()" style="color: MediumSeaGreen; margin-right: 10px;">Run</button>
            <button class="button" onclick="kill()" style="color: Tomato;" >Kill</button>
        </div>
    </body>

    <!-- JAVA SCRIPT SECTION -->
    <script type="text/javascript">
        let currentView = "Manual Mode"; // Track current view
        var motorCount = 0; // Track number of motors added dynamically
        
        //microfluidics webdisplay functions
        function setAllMotors(){
            const MRVRaw = document.getElementById("MotorRotationalVelocityAll").value;
            const motorInputs = document.querySelectorAll('[id^="MRV_manual"]');

            motorInputs.forEach(input => {
                input.value = MRVRaw;
            });
            
            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_ALL_MOTORS", false); //this false means synchronous
            xhttp.send();
        }

        function updateAllMRVControl( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            //Modify html values using IDs
            document.getElementById("MotorRotationalVelocityAll").value = MRVRaw;

            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "UPDATE_MOTOR_ALL_CONTROL?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

    
        function setCheckboxesOn(){
            const checkboxes = document.querySelectorAll('[id^="checkbox"]');
    
            checkboxes.forEach(cb => {
                cb.checked = true;
            });

            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOXES_ON", false); //this false means synchronous
            xhttp.send();
        }

        function setCheckboxesOff(){
            const checkboxes = document.querySelectorAll('[id^="checkbox"]');
    
            checkboxes.forEach(cb => {
                cb.checked = false;
            });
            
            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOXES_OFF", false);
            xhttp.send();
        }

        //<toggle checkboxes>
        function toggleCheckbox(checkboxElem) {
            const strid = checkboxElem.id.match(/\d+$/); // grab digits from checkbox id, e.g. "checkbox3" -> ["3"]
            const motorNum = strid[0]; // use the first and only match
            const isChecked = checkboxElem.checked;

            // Build the URL dynamically based on checkbox id
            const url = `TOGGLE_CHECKBOX?MOTORNUM=${motorNum}&STATE=${isChecked}`;

            // Use XMLHttpRequest synchronously (like your original code)
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", url, false);  // synchronous request (deprecated but matches your style)
            xhttp.send();
        }
        //</toggle checkboxes>
        //<update MRV>
        function updateMRV(inputElem) {
            const strid = inputElem.id.match(/\d+$/); //str ends in a number
            const motorNum = strid[0]; // use the first and only match
            let MRVRaw = inputElem.value;
            MRVRaw = rangeRestrictionMVR(MRVRaw); // Ensure MRVRaw is within the specified range
            document.getElementById(inputElem.id).value = MRVRaw; // Update the input value

            // Build the URL dynamically based on input id
            const url = `SET_MRV?MOTORNUM=${motorNum}&VALUE=${MRVRaw}`;

            // Use XMLHttpRequest synchronously (like your original code)
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", url, false);  // synchronous request (deprecated but matches your style)
            xhttp.send();
        }
        
        //</update MRV>

        function run(){
            var xhttp = new XMLHttpRequest();
            if(currentView === "Manual Mode"){
                xhttp.open("PUT", "MANUAL_RUN", true); //this false means synchronous
            }
            else if(currentView === "Scheduling Mode"){
                const totalEllapseTimeS = parseFloat(document.getElementById("ellapseTimeMotorSwitchingHours").value) * 3600 +
                                parseFloat(document.getElementById("ellapseTimeMotorSwitchingMinutes").value) * 60 +
                                parseFloat(document.getElementById("ellapseTimeMotorSwitchingSeconds").value);
                const ellapseTimeMS = totalEllapseTimeS * 1000; //convert to milliseconds
                if( ellapseTimeMS <= 0 ){
                    alert("Please set a positive elapsed time.");
                    return;
                }
                const dateTime = document.getElementById("dateTime").value;

                const targetDate = new Date(dateTime);// create date object

                if (isNaN(targetDate.getTime())) {
                    alert("Invalid dateTime entered, NaN error");
                    return;
                }

                const scheduledDateTimeStampMS = targetDate.getTime(); //save scheduled time

                const currentUTCMS = new Date().getTime(); //Grab current time

                if (scheduledDateTimeStampMS <= currentUTCMS) {
                    alert("Please choose a future Datetime.");
                    return;
                }

                // update 
                updateScheduleOnHost(ellapseTimeMS, scheduledDateTimeStampMS); //stores schedule and ellapse on esp32 c++ global variables

                xhttp.open("PUT", "SCHEDULE_RUN", true); //this true means not synchronous
            }
            xhttp.send();
        }
        function kill(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "KILL", false); //this false means synchronous
            xhttp.send(currentView);
        }
        
        //function to restrict MVR values to the range of -90000 to 90000
        /*currently hardcoded so if you have to modify this value
          to reflect the global range specified in the main file. */
        function rangeRestrictionMVR( MVRRaw ){
            if( MVRRaw < -90000 ){ //this value is hardcoded to reflect the range restrictions set in the main c script.
                MVRRaw = -90000;
            }
            else if( MVRRaw > 90000 ){
                MVRRaw = 90000;
            }
            return MVRRaw;
        }

        function showView(view) {
            var mainView = document.getElementById("manualMode");
            var secondaryView = document.getElementById("schedulingMode");
            
            if (view === "Manual Mode") {
                mainView.style.display = "block";
                secondaryView.style.display = "none";
            } else if (view === "Scheduling Mode") {
                mainView.style.display = "none";
                secondaryView.style.display = "block";
            }

            currentView = view; //Track current view
            reset(); //clears all data in website
        }

        function reset(){
            var xhttp = new XMLHttpRequest();
            // Reset all motor velocities (1 through 7) and the "All" field
            document.getElementById("MotorRotationalVelocityAll").value = 0;
            for (let i = 1; i <= motorCount; i++) {
                document.getElementById("MotorRotationalVelocity" + i).value = 0;
            }
            // Reset checkboxes (1 through 7)
            for (let i = 1; i <= motorCount; i++) {
                document.getElementById("checkbox_manual" + i).checked = false;
                document.getElementById("checkbox_schedule" + i).checked = false;
            }
            xhttp.open("PUT", "RESET", false); //this false means synchronous
            xhttp.send(currentView);
        }

        function verifyRangeMRV( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("scheduledMRV").value = MRVRaw;
        }

        function updateScheduleOnHost(ellapseTimeMS, scheduledDateTimeStampMS){
            var xhttpDateTime = new XMLHttpRequest();
            xhttpDateTime.open("PUT", "SCHEDULED_DATE_TIME?VALUE="+scheduledDateTimeStampMS, false); //this false means synchronous
            xhttpDateTime.send();
            var xhttpEllapse = new XMLHttpRequest();
            xhttpEllapse.open("PUT", "SCHEDULE_ELLAPSE_TIME?VALUE="+ellapseTimeMS, false); //this false means synchronous
            xhttpEllapse.send();
        }

        function checkEllapseTime(){
            var warningEllapseTime = document.getElementById("ellapseTimeWarning");
            const totalEllapseTimeS = parseFloat(document.getElementById("ellapseTimeMotorSwitchingHours").value) * 3600 +
                                parseFloat(document.getElementById("ellapseTimeMotorSwitchingMinutes").value) * 60 +
                                parseFloat(document.getElementById("ellapseTimeMotorSwitchingSeconds").value);
            if( totalEllapseTimeS <= 0 ){
                warningEllapseTime.style.display = "inline"; // show warning
            }
            else{
                warningEllapseTime.style.display = "none"; // hide warning
            }
        }
        // Dynamic implemntation
        function addMotors( desiredMotorCount ) {
            while (motorCount < desiredMotorCount) {
                addMotor();
            }
            while (motorCount > desiredMotorCount) {
                removeMotor();
            }
        }

        function addMotor(){
            var xhttp = new XMLHttpRequest();

            const mySectionManual = document.getElementById("motorControlsManual");
            const mySectionSchedule = document.getElementById("motorControlsSchedule");
            motorCount++; // Current Added Motor Count
            const motorConfigManual = `<div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox_manual${motorCount}" onchange="toggleCheckbox(this)"> Motor ${motorCount}</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MRV_manual${motorCount}" onchange="updateMRV(this)"></p>
                </div>`;
            const motorConfigSchedule = `<div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox_schedule${motorCount}" onchange="toggleCheckbox(this)"> Motor ${motorCount}</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MRV_schedule${motorCount}" onchange="updateMRV(this)"></p>
                </div>`;
            mySectionManual.insertAdjacentHTML('beforeend', motorConfigManual); // Add new motor content
            mySectionSchedule.insertAdjacentHTML('beforeend', motorConfigSchedule); // Add new motor content to schedule section

            document.getElementById("numMotors").value = motorCount;

            xhttp.open("PUT", "ADD_MOTORS?VALUE="+motorCount, true); //this false means synchronous
            xhttp.send();
        }

        function removeMotor(){
            var xhttp = new XMLHttpRequest();

            const mySectionManual = document.getElementById("motorControlsManual");
            const mySectionSchedule = document.getElementById("motorControlsSchedule");
                //turn last child off before removing it!
            if (mySectionManual.children.length > 0) {
                mySectionManual.removeChild(mySectionManual.lastChild); // Remove the last motor
                mySectionSchedule.removeChild(mySectionSchedule.lastChild); // Remove the last motor from schedule section
                
                motorCount--; // Get current count of motors
                xhttp.open("PUT", "ADD_MOTORS?VALUE="+motorCount, true); //this false means synchronous
                xhttp.send();
            }

            document.getElementById("numMotors").value = motorCount;

        }

    </script>
</html>
)rawliteral";