const char PAGE_MAIN[] PROGMEM = R"rawliteral(
<!-- listening for PAGE_MAIN server connection -->
<!-- PROGMEM specifies that PAGE_MAIN is stored in flash memory to conserve space in RAM. -->
<!-- R"rawliteral means store the array as written for formatting embedded HTML -->
<!-- html section -->
<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
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
            width: 100%;
            margin-top: 15px;
        }
        td, th {
            border: 1px solid #dddddd;
            text-align: center;
            padding: 8px;
        }
        tr:nth-child(even) {
            background-color: #dddddd;
        }
        .velocity-input {
            width: 50px;
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
            <p style="color: rgb(255, 255, 255); font-size: 10px;">Accepted Range {-90000, 90000}</p>
        
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
            <div class="wrapper">
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox1" onchange="togglecheckbox1(this.checked)"> Motor 1</p><!-- -->
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity1" onchange="updateMRV1(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox2" onchange="togglecheckbox2(this.checked)"> Motor 2</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity2" onchange="updateMRV2(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox3" onchange="togglecheckbox3(this.checked)"> Motor 3</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity3" onchange="updateMRV3(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox4" onchange="togglecheckbox4(this.checked)"> Motor 4</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity4" onchange="updateMRV4(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox5" onchange="togglecheckbox5(this.checked)"> Motor 5</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity5" onchange="updateMRV5(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox6" onchange="togglecheckbox6(this.checked)"> Motor 6</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity6" onchange="updateMRV6(this.value)"></p>
                </div>
                <div class="myDiv">
                    <p style="color: rgb(255, 255, 255);"><input type="checkbox" id="checkbox7" onchange="togglecheckbox7(this.checked)"> Motor 7</p>
                    <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity7" onchange="updateMRV7(this.value)"></p>
                </div>                                                                                          <!-- -->
            </div>

            <!-- Run and Kill buttons-->
            <div class="wrapper" style="margin-top: 10px;">
                <button class="button" onclick="run()" style="color: MediumSeaGreen; margin-right: 10px;">Run</button>
                <button class="button" onclick="kill()" style="color: Tomato;" >Kill</button>
            </div>
        </div>


<!-- Schedule Mode Section! -->
        <div id="schedulingMode" style="display: none;">
            <h2 class="title">Scheduling Mode</h2>
            <!-- Changing Time Buttons -->
            <div style="text-align: center">
                <button onclick="showTimeView('AbsoluteTime')">AbsoluteTime</button>
                <button onclick="showTimeView('ClockTime')">24ClockTime</button>
                <button onclick="showTimeView('DateTime')">DateTime</button>
            </div>
            <!-- Table -->
            <table>
                <tr>
                  <th>Motors</th>
              
                  <!-- Dynamic header swapping -->
                  <th>
                    <div id="timeHeader-Absolute" style="display: block;">AbsoluteTime</div>
                    <div id="timeHeader-Clock" style="display: none;">24ClockTime</div>
                    <div id="timeHeader-DateTime" style="display: none;">DateTime</div>
                  </th>
              
                  <th>Velocity (ω)</th>
                </tr>
              
                <!-- Motor 1 -->
                <tr>
                  <td>Motor 1</td>
                  <td>
                    <div class="timeModes">
                      <div class="AbsoluteTime" style="display: block;">
                        <div style="display: inline-flex; align-items: center;">
                          <input type="number" min="0" placeholder="HH" style="width: 40px; text-align: center;">
                          <span>:</span>
                          <input type="number" min="0" max="59" placeholder="MM" style="width: 40px; text-align: center;">
                          <span>:</span>
                          <input type="number" min="0" max="59" placeholder="SS" style="width: 40px; text-align: center;">
                        </div>
                      </div>
                      <div class="ClockTime" style="display: none;">
                        <input type="time" step="1" style="width: 116px;">
                      </div>
                      <div class="DateTime" style="display: none;">
                        <input type="datetime-local" style="width: 170px;">
                      </div>
                    </div>
                  </td>
                  <td><input type="number" class="velocity-input" value="0"></td>
                </tr>
              
                <!-- Motor 2 -->
                <tr>
                  <td>Motor 2</td>
                  <td>
                    <div class="timeModes">
                      <div class="AbsoluteTime" style="display: block;">
                        <div style="display: inline-flex; align-items: center;">
                          <input type="number" min="0" placeholder="HH" style="width: 40px; text-align: center;">
                          <span>:</span>
                          <input type="number" min="0" max="59" placeholder="MM" style="width: 40px; text-align: center;">
                          <span>:</span>
                          <input type="number" min="0" max="59" placeholder="SS" style="width: 40px; text-align: center;">
                        </div>
                      </div>
                      <div class="ClockTime" style="display: none;">
                        <input type="time" step="1" style="width: 116px;">
                      </div>
                      <div class="DateTime" style="display: none;">
                        <input type="datetime-local" style="width: 170px;">
                      </div>
                    </div>
                  </td>
                  <td><input type="number" class="velocity-input" value="0"></td>
                </tr>
              
              </table>
        </div>
    </body>

    <!-- JAVA SCRIPT SECTION -->
    <script type="text/javascript">
        
        //microfluidics webdisplay functions
        function setAllMotors(){
            //Modify variables in main script to reflect html changes
            MRVRaw = document.getElementById("MotorRotationalVelocityAll").value;
            document.getElementById("MotorRotationalVelocity1").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity2").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity3").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity4").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity5").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity6").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity7").value = MRVRaw;

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
            //Modify html values using IDs
            document.getElementById("checkbox1").checked = true;
            document.getElementById("checkbox2").checked = true;
            document.getElementById("checkbox3").checked = true;
            document.getElementById("checkbox4").checked = true;
            document.getElementById("checkbox5").checked = true;
            document.getElementById("checkbox6").checked = true;
            document.getElementById("checkbox7").checked = true;

            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOXES_ON", false); //this false means synchronous
            xhttp.send();
        }

        function setCheckboxesOff(){
            //Modify html values using IDs
            document.getElementById("checkbox1").checked = false;
            document.getElementById("checkbox2").checked = false;
            document.getElementById("checkbox3").checked = false;
            document.getElementById("checkbox4").checked = false;
            document.getElementById("checkbox5").checked = false;
            document.getElementById("checkbox6").checked = false;
            document.getElementById("checkbox7").checked = false;
            
            //create XMLHttpRequest object to send data to the server
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOXES_OFF", false);
            xhttp.send();
        }

        //<toggle checkboxes>
        function togglecheckbox1( checkState ){
            document.getElementById("checkbox1").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX1?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox2( checkState ){
            document.getElementById("checkbox2").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX2?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox3( checkState ){
            document.getElementById("checkbox3").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX3?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox4( checkState ){
            document.getElementById("checkbox4").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX4?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox5( checkState ){
            document.getElementById("checkbox5").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX5?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox6( checkState ){
            document.getElementById("checkbox6").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX6?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }

        function togglecheckbox7( checkState ){
            document.getElementById("checkbox7").checked = checkState;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_CHECKBOX7?STATE="+checkState, false); //this false means synchronous
            xhttp.send();
        }
        //</toggle checkboxes>
        //<update MRV>
        function updateMRV1( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity1").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV1?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV2( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity2").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV2?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV3( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity3").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV3?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV4( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity4").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV4?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV5( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity5").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV5?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV6( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity6").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV6?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }

        function updateMRV7( MRVRaw ){
            MRVRaw = rangeRestrictionMVR(MRVRaw)
            document.getElementById("MotorRotationalVelocity7").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SET_MRV7?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }
        //</update MRV>

        function run(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "RUN", false); //this false means synchronous
            xhttp.send();
        }
        function kill(){
            var xhttp = new XMLHttpRequest();

            xhttp.open("PUT", "KILL", false); //this false means synchronous
            xhttp.send();
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
        }

        // Scheduling mode functions:
        function showTimeView(view) {
            // Header toggle
            document.getElementById("timeHeader-Absolute").style.display = (view === "AbsoluteTime") ? "block" : "none";
            document.getElementById("timeHeader-Clock").style.display = (view === "ClockTime") ? "block" : "none";
            document.getElementById("timeHeader-DateTime").style.display = (view === "DateTime") ? "block" : "none";

            // Time cell toggle for each motor
            const modes = ["AbsoluteTime", "ClockTime", "DateTime"];
            modes.forEach(mode => {
            const elements = document.getElementsByClassName(mode);
            for (let el of elements) {
                el.style.display = (view === mode) ? "block" : "none";
            }
            });
        }

        function scheduleMotor1(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SCHEDULE_MOTOR_1", false); //this false means synchronous
            xhttp.send();
        }
        function scheduleMotor2(){
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "SCHEDULE_MOTOR_2", false); //this false means synchronous
            xhttp.send();
        }
    </script>
</html>
)rawliteral";