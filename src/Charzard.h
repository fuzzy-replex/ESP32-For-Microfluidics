#ifndef CHARZARD_H
#define CHARZARD_H

const char PAGE_MAIN[] = R"rawliteral(
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
        color: #000000;
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

    <body style="Background-color: #F0F0F0F0" onload="process()">
        <div>
            class="title">Microflidics Pump Controller
        </div>
        <p>Motor Rotational Velocity:</p>
        <p style="font-size:10px;">Accepted Range {-90000, 90000}</p>
    
        <!-- Modify all GUI-->
        <div class="wrapper">
            <div class="myDiv">
                <p><button>Set All Motors</button></p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocityAll" onchange="updateAllMRV(this.value)"><p>
            </div>
            <div class="myDiv">
                <p><button onclick="setCheckboxesOn()">Set Checkboxes On</button></p>
                <p><button onclick="setCheckboxesOff()">Set Checkboxes Off</button></p>
            </div>
        </div>

        <!-- Motors 1 though 7 GUI-->
        <div class="wrapper">
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox1()"> Motor 1</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity1" onchange="updateMRV1()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox2()"> Motor 2</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity2" onchange="updateMRV2()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox3()"> Motor 3</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity3" onchange="updateMRV3()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox4()"> Motor 4</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity4" onchange="updateMRV4()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox5()"> Motor 5</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity5" onchange="updateMRV5()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox6()"> Motor 6</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity6" onchange="updateMRV6()"></p>
            </div>
            <div class="myDiv">
                <p><input type="checkbox" id="checkbox1" onchange="togglecheckbox7()"> Motor 7</p>
                <p><input type="number" min="-90000" max="90000" value="0" id="MotorRotationalVelocity7" onchange="updateMRV7()"></p>
            </div>
        </div>

        <!-- Run and Kill buttons-->
        <p><a href="/run"><button class="button" style="color: MediumSeaGreen">Run</button></a></p>
        <p><a href="/kill"><button class="button" style="color: Tomato;">Kill</button></a></p>
    </body>

    <!-- JAVA SCRIPT SECTION -->
    <script type="text/javascript">
        //this is global
        var xmlHttp = createXmlHttpObject();
        
        //functions to deal wth sending data between server and client
        function createXmlHttpObject() {
            if (window.XMLHttpRequest) {
                xmlHttp = new XMLHttpRequest();
            } else {
                xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
            }
            return xmlHttp;
        }
  
        // runs when body loads
        function process() {
            if (xmlHttp.readyState == 0 || xmlHttp.readyState == 4) {
                xmlHttp.open("PUT", "xml", true);
                xmlHttp.onreadystatechange = response;
                xmlHttp.send(null);
            }
            setTimeout("process()", 100);
        }

        function response() {
            var message;
        }

        //functions for my server logic
        function setCheckboxesOn(){
            document.getElementById("checkbox1").checked = true;
            document.getElementById("checkbox2").checked = true;
            document.getElementById("checkbox3").checked = true;
            document.getElementById("checkbox4").checked = true;
            document.getElementById("checkbox5").checked = true;
            document.getElementById("checkbox6").checked = true;
            document.getElementById("checkbox7").checked = true;
            
            var xhttp = new XMLHttpRequest();
            var message;
            xhttp.open("PUT", "SET_BUTTONS_ON", false); //this false means synchronous
            xhttp.send();
        }

        function setCheckboxesOff(){
            document.getElementById("checkbox1").checked = false;
            document.getElementById("checkbox2").checked = false;
            document.getElementById("checkbox3").checked = false;
            document.getElementById("checkbox4").checked = false;
            document.getElementById("checkbox5").checked = false;
            document.getElementById("checkbox6").checked = false;
            document.getElementById("checkbox7").checked = false;
            
            var xhttp = new XMLHttpRequest();
            var message;
            xhttp.open("PUT", "SET_BUTTONS_OFF", false);
            xhttp.send();
        }

        function updateAllMRV( MRVRaw ){
            document.getElementById("MotorRotationalVelocity1").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity2").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity3").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity4").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity5").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity6").value = MRVRaw;
            document.getElementById("MotorRotationalVelocity7").value = MRVRaw;

            var xhttp = new XMLHttpRequest();
            var message;
            xhttp.open("PUT", "SET_MOTORS?VALUE="+MRVRaw, false); //this false means synchronous
            xhttp.send();
        }


    </script>

</html>
)rawliteral";

#endif