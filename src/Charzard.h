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
            height: 100%;
        }
        td, th {
            border: 1px solid #b7b7b7;
            text-align: center;
            padding: 8px;
            color: white;
            height: 100%;
        }
        th.datetime-col, td.datetime-col {
            width: 170px;
        }
        th.motor-col, td.motor-col {
            min-width: 100px;
            max-width: 420px;
            width: auto;
        }
        th.elapse-col, td.elapse-col {
            width: 200px;
        }
        tr:nth-child(even) {
            background-color: #434343;
            height: 100%;
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
        }/* Make the wrapper fill its parent table cell */
        .elapse-col {
            height: 100%;          /* let the td expand */
        }

        .elapse-wrapper {
            flex: 1;
            display: flex;
            flex-direction: column;
            justify-content: space-evenly; /* spread items top → bottom */
            height: 100%;                   /* take full height of parent td */
        }

        .elapse-subgroup {
            flex: 1;                         /* each subgroup takes equal share */
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .time-inputs {
            display: flex;
            align-items: center;
            gap: 6px; /* equal spacing between inputs */
        }

        .time-inputs input {
            width: 40px;
            text-align: center;
        }
        .elapse-subgroup hr {
            width: 100%;
            border: none;
            border-top: 1px solid #aaa;  /* or white depending on your theme */
            margin: 8px 0;
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
            <div style="align-items: center; justify-content: center;"> <!-- Dynamically change motor num numbox -->
                <label for="numMotors" style="color: white;">Number of Motors:</label>
                <input type="number" min="0" max="8" value="0" id="numMotors" onchange="addMotors(this.value)"></input>
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

            <!-- Motors 1 though 7 GUI-->
            <div class="wrapper" id="motorControlsManual">
                <!-- dynamically allocated using js HERE using topnav-->
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

                    <th class="motor-col">
                        <div>
                            MRV <small><small></small>(<sup>&mu;_steps</sup>&frasl;<sub>sec</sub><small></small>)
                        </div>
                        <div style="font-size: 0.6em;">
                            [-90000, 90000]
                        </div>
                    </th>

                    <th>
                        <div class="elapse-col crumpleByWord">
                            Elapse Time
                        </div>
                        <div style="font-size: 0.6em;">
                            (Motor on durration)
                        </div>
                    </th>
                </tr>
                <!-- Time Specs -->
                <tr>
                    <td class="datetime-col">
                        <input id="dateTime" type="datetime-local" style="width: 170px;">
                    </td>
                    <td class="motor-col">
                        <div id="motorSubGroups_0">
                            <!-- SubGroup 0 -->
                            <div class="motor-subgroup" id="motorGroup_0" style="margin-bottom: 10px;">
                                <div class="wrapper">
                                    <!-- dynamically allocated motor nums js HERE using topnav-->
                                </div>
                            </div>
                            <!-- JS dynamically adds Subgroups -->
                        </div>
                    </td>
                    <td class="elapse-col">
                        <div id="elapseSubGroups" class="elapse-wrapper">
                            <!-- SubGroup 0 -->
                            <div class="elapse-subgroup" id="elapseGroup_0" style="align-items: center;">
                                <div style="display: flex; align-items: center; justify-content: center;">
                                    <input id="elapseGroup_0_mins" style="width:40px;" type="number" value="0" min="0" oninput="updateElapseTime()"> mins&nbsp;:&nbsp;
                                    <input id="elapseGroup_0_secs" style="width:40px;" type="number" value="0" min="0" oninput="updateElapseTime()"> secs
                                </div>
                            </div>
                            <!-- JS dynamically adds Subgroups -->
                        </div>
                    </td>
                    <!-- Dynamically Add new Motor Modify and ElapseTime -->
                    <!-- End Dynamically Add new Motor Modify and ElapseTime -->
                </tr>
            </table>
            <div style="display: flex; justify-content: center; gap: 40px; margin-top: 10px;">
                <div style="display: block;">
                    <button onclick="addMotorElapseGroups()">+ Add Elapse Field</button>
                    <button onclick="removeMotorElapseGroups()">- Remove Elapse Field</button>
                </div>
            </div>
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
        var subgroupCount = 1; // Track number of motor subgroups in scheduling mode
        
        //microfluidics webdisplay functions
        function setAllMotors(){
            const MRVRaw = document.getElementById("MotorRotationalVelocityAll").value;
            const motorInputs = document.querySelectorAll('[id^="MRV_manual"]');

            motorInputs.forEach(input => {
                input.value = MRVRaw;
            });
            
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_ALL_MOTORS", false); //synchronous
            xhttp.send();
        }

        function updateAllMRVControl( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            //Modify html values using IDs
            document.getElementById("MotorRotationalVelocityAll").value = MRVRaw;

            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "UPDATE_MOTOR_ALL_CONTROL?VALUE="+MRVRaw, false); 
            xhttp.send();
        }

    
        function setCheckboxesOn(){
            const checkboxes = document.querySelectorAll('[id^="checkbox_manual"]');
    
            checkboxes.forEach(cb => {
                cb.checked = true;
            });

            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOXES_ON", false); //synchronous
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
            const isChecked = checkboxElem.checked;
            let url = "";
            if(currentView === "Manual Mode"){
                const strid = checkboxElem.id.match(/\d+/); // parse motor number from id "checkbox_manualX"
                const motorNum = parseInt(strid[0], 10); // Convert to integer
                url = `TOGGLE_CHECKBOX?SUBGROUP_INDEX=${0}&MOTORNUM=${motorNum}&STATE=${isChecked}`;
            }
            else{ //scheduling mode
                const strid = checkboxElem.id.match(/\d+_\d+/); // parse 2d dynamic array with checkbox state.
                const [subgroupIndex, motorNum] = strid[0].split('_').map(Number);
                url = `TOGGLE_CHECKBOX?SUBGROUP_INDEX=${subgroupIndex}&MOTORNUM=${motorNum}&STATE=${isChecked}`;
            }
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", url, false);  // synchronous request
            xhttp.send();
        }
        //</toggle checkboxes>
        //<update MRV>
        function updateMRV(inputElem) {
            let motorNum;
            let subgroupIndex;
            if(currentView === "Manual Mode"){
                const strid = inputElem.id.match(/\d+/); // parse motor number from id "MRV_manualX"
                motorNum = parseInt(strid[0], 10); // Convert to integer
                subgroupIndex = 0; // Manual mode only uses subgroup 0
            }
            else{ //scheduling mode
                const strid = inputElem.id.match(/\d+_\d+/); // parse 2d dynamic array with checkbox state.
                [subgroupIndex, motorNum] = strid[0].split('_').map(Number);
            }
            let MRVRaw = inputElem.value;
            MRVRaw = rangeRestrictionMVR(MRVRaw); // Ensure MRVRaw is within the specified range
            document.getElementById(inputElem.id).value = MRVRaw; // Update the input value

            // Build the URL dynamically based on input id
            const url = `SET_MRV?SUBGROUP_INDEX=${subgroupIndex}&MOTORNUM=${motorNum}&VALUE=${MRVRaw}`;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", url, false);  // synchronous request
            xhttp.send();
        }
        
        //</update MRV>

        function run(){
            var xhttp = new XMLHttpRequest();
            if(currentView === "Manual Mode"){
                xhttp.open("PUT", "MANUAL_RUN", true);
            }
            else if(currentView === "Scheduling Mode"){
                // Validate datetime and elapse times
                const totalelapseTimeS = parseFloat(document.getElementById("elapseGroup_0_mins").value) * 60 +
                                          parseFloat(document.getElementById("elapseGroup_0_secs").value);
                if( totalelapseTimeS <= 0 ){
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

                //convert to milliseconds
                const elapseTimeMS_start = totalelapseTimeS * 1000;
                updateScheduleOnHost(elapseTimeMS_start, scheduledDateTimeStampMS); //stores schedule and elapse on esp32 c++ global variables

                xhttp.open("PUT", "SCHEDULE_RUN", false); //synchronous
            }
            xhttp.send();
        }
        function kill(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "KILL", false); //synchronous
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
            
            const motorInputs = document.querySelectorAll('[id^="MRV"]');
            motorInputs.forEach(input => input.value = 0);

            const manualBoxes = document.querySelectorAll('[id^="checkbox"]');
            manualBoxes.forEach(cb => cb.checked = false);

            xhttp.open("PUT", "RESET?VIEW="+currentView, false); //synchronous
            xhttp.send();
        }

        function verifyRangeMRV( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("scheduledMRV").value = MRVRaw;
        }

        function updateScheduleOnHost(elapseTimeMS_start, scheduledDateTimeStampMS){
            var xhttpDateTime = new XMLHttpRequest();
            xhttpDateTime.open("PUT", "SCHEDULED_DATE_TIME?VALUE="+scheduledDateTimeStampMS, false);
            xhttpDateTime.send();
        }

        // Dynamic implemntation
        function addMotors( desiredMotorCount ) {
            if (desiredMotorCount > 8) {
                document.getElementById("numMotors").value = 8;
                desiredMotorCount = 8; // Max limit
            }
            else if (desiredMotorCount < 0) {
                document.getElementById("numMotors").value = 0;
                desiredMotorCount = 0; // Min limit
            }
            while (motorCount < desiredMotorCount) {
                addMotor();
            }
            while (motorCount > desiredMotorCount) {
                removeMotor();
            }
        }
        //issue where motorcount is updated in schedule!!!

        function addMotor(){
            var xhttp = new XMLHttpRequest();
            motorCount++; // Current Added Motor Count

            const mySectionManual = document.getElementById("motorControlsManual");
            const motorConfigManual = `<div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox_manual${motorCount}" onchange="toggleCheckbox(this)"> Motor ${motorCount}</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MRV_manual${motorCount}" onchange="updateMRV(this)"></p>
                </div>`;
            mySectionManual.insertAdjacentHTML('beforeend', motorConfigManual); // Add new motor content

            
            const mySectionSchedule = document.querySelectorAll(".motor-subgroup > .wrapper");
            
            
            mySectionSchedule.forEach((wrapper, subgroupIndex) => {
                const parentId = wrapper.parentElement.id; // e.g. motorGroup_0_0
                const ids = parentId.split("_");
                const subIndex = ids[1];

                const customizedScheduleHTML = `
                <div class="myDiv">
                    <p style="color: white;">
                        <input type="checkbox" id="checkbox_schedule_${subIndex}_${motorCount}" onchange="toggleCheckbox(this)"> Motor ${motorCount}
                    </p>
                    <p>
                        <input type="number" min="-90000" max="90000" value="0"
                            id="MRV_schedule_${subIndex}_${motorCount}"
                            onchange="updateMRV(this)">
                    </p>
                </div>`;

                wrapper.insertAdjacentHTML("beforeend", customizedScheduleHTML);
            });
            document.getElementById("numMotors").value = motorCount;

            xhttp.open("PUT", "ADD_MOTORS?VALUE="+motorCount, true); 
            xhttp.send();
        }

        function removeMotor() {
            var xhttp = new XMLHttpRequest();

            // Remove from Manual Mode
            const mySectionManual = document.getElementById("motorControlsManual");
            if (mySectionManual.children.length > 0) {
                mySectionManual.removeChild(mySectionManual.lastChild);
            }

            // Remove from every Scheduled Subgroup
            const allScheduleWrappers = document.querySelectorAll(".motor-subgroup > .wrapper");
            allScheduleWrappers.forEach(wrapper => {
                if (wrapper.children.length > 0) {
                    wrapper.removeChild(wrapper.lastElementChild); // remove the last motor div
                }
            });

            // Decrement count and sync with ESP
            if (motorCount > 0) {
                motorCount--;
                document.getElementById("numMotors").value = motorCount;

                xhttp.open("PUT", "ADD_MOTORS?VALUE=" + motorCount, true); // Send new count to ESP
                xhttp.send();
            }
        }


        function addMotorElapseGroups(){
            addMotorSubGroup();
            addelapseSubGroup();
            subgroupCount++;
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "ADD_ELAPSE_FIELD?SG_COUNT=" + subgroupCount, false); //synchronous
            xhttp.send();
        }

        function addMotorSubGroup() {
            const subgroupCount = motorWrapper.querySelectorAll(".motor-subgroup").length;

            const motorGroupId = `motorGroup_${subgroupCount}`;
            let html = `<div class="motor-subgroup" id="${motorGroupId}"><div class="wrapper">`;

            html += '<hr style="border: none; border-top: 1px solid white; width: 100%; margin: 10px 0;">';
            for (let i = 1; i <= motorCount; i++) {
                html += `
                <div class="myDiv">
                    <p style="color: white; text-align: center;">
                    <input type="checkbox" id="checkbox_schedule_${subgroupCount}_${i}" onchange="toggleCheckbox(this)"> Motor ${i}
                    </p>
                    <p>
                    <input type="number" min="-90000" max="90000" value="0"
                        id="MRV_schedule_${subgroupCount}_${i}"
                        onchange="updateMRV(this)">
                    </p>
                </div>`;
            }

            html += `</div></div>`;
            motorWrapper.insertAdjacentHTML("beforeend", html);
        }

        function addelapseSubGroup() {
            const elapseWrapper = document.getElementById(`elapseSubGroups`);
            const subgroupCount = elapseWrapper.querySelectorAll(".elapse-subgroup").length;
            const html = `
                <hr style="border: none; border-top: 1px solid white; width: 100%; margin: 10px 0;">
                <div class="elapse-subgroup" id="elapseGroup_${subgroupCount}" style="margin-bottom: 10px;">
                <div style="display: flex; align-items: center; justify-content: center;">
                    <input id="elapseGroup_${subgroupCount}_mins" style="width:40px;" type="number" value="0" min="0" oninput="updateElapseTime()"> mins&nbsp;:&nbsp;
                    <input id="elapseGroup_${subgroupCount}_secs" style="width:40px;" type="number" value="0" min="0" oninput="updateElapseTime()"> secs&nbsp;:&nbsp;
                </div>`;

            elapseWrapper.insertAdjacentHTML("beforeend", html);
        }

        function updateElapseTime() {
            const strid = event.target.id.match(/\d+/); // parse group number from id "elapseGroup_X_mins" or "elapseGroup_X_secs"
            const subgroupIndex = parseInt(strid[0], 10); // Convert to integer

            const mins = parseInt(document.getElementById(`elapseGroup_${subgroupIndex}_mins`).value);
            const secs = parseInt(document.getElementById(`elapseGroup_${subgroupIndex}_secs`).value);

            if( mins < 0 || secs < 0 ){
                alert("Please enter non-negative values for minutes and seconds.");
                return;
            }

            const totalElapseTimeMS = (mins * 60 + secs) * 1000; // Convert to milliseconds

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", `UPDATE_ELAPSE_TIME?SUBGROUP_INDEX=${subgroupIndex}&ELAPSE_TIME=${totalElapseTimeMS}`, false);
            xhttp.send();
        }

        //needs to keep track of which row it's tied to
        function removeMotorElapseGroups() {

            const motorGroups = motorWrapper.querySelectorAll(".motor-subgroup");
            const elapseGroups = elapseWrapper.querySelectorAll(".elapse-subgroup");

            if(motorGroups.length === 1 && elapseGroups.length === 1){
                return; // Prevent removing the last subgroup
            }

            // Only remove if both have at least one subgroup
            if (motorGroups.length > 0 && elapseGroups.length > 0) {
                motorWrapper.removeChild(motorGroups[motorGroups.length - 1]);
                elapseWrapper.removeChild(elapseGroups[elapseGroups.length - 1]);
                elapseWrapper.removeChild(elapseWrapper.lastElementChild); // remove the last <hr>
            }
            if(subgroupCount > 1){
                subgroupCount--;
            }
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "REMOVE_ELAPSE_FIELD?SG_COUNT=" + subgroupCount, false);
            xhttp.send();
        }

    </script>
</html>
)rawliteral";