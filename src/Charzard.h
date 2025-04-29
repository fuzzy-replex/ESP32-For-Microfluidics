const char PAGE_MAIN[] PROGMEM = R"rawliteral(
<!-- listening for PAGE_MAIN server connection -->
<!-- PROGMEM specifies that PAGE_MAIN is stored in flash memory to conserve space in RAM. -->
<!-- R"rawliteral means store the array as written for formatting embedded HTML -->
<!-- html section -->
<!DOCTYPE html>
<html lang="en" class="js-focus-visible">
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
    </style>
    <div class="header">
        <h1 style="color: rgb(255, 255, 255);">Microfluidics Pump Controller</h1>
    </div>

    <body style="Background-color: #383838">
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

    </script>
</html>
)rawliteral";