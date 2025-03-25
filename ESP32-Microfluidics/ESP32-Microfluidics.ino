#include <WiFi.h>
#include <string>

// Replace with your network credentials
const char* ssid     = "YourInAComaWakeUp";
const char* password = "WakeUpNathan";

// Set web server port number to 80
WiFiServer server(80);

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

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output23, OUTPUT);
  pinMode(output22, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output23, LOW);
  digitalWrite(output22, LOW);

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
  server.begin();
}

int motorTurnVelocityRaw = 0;
int motorTurnVelocity255 = 0;
bool checkbox0State = false;
bool checkbox1State = false;
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();   

      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;

        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Update checkbox#States
            if (header.indexOf("GET /setcheckbox0?state=") >= 0) {
                String stateString = header.substring(header.indexOf("state=") + 6);
                checkbox0State = stateString.toInt(); //updates checkbox0 to 0 off or 1 on.
            }
            if (header.indexOf("GET /setcheckbox1?state=") >= 0) {
                String stateString = header.substring(header.indexOf("state=") + 6);
                checkbox1State = stateString.toInt(); //updates checkbox1 to 0 off or 1 on.
            }

            // Update motorTurnVelocity
            if (header.indexOf("GET /setBrightness23?value=") >= 0) {
              motorTurnVelocityRaw = header.substring(header.indexOf("value=") + 6).toInt();
              motorTurnVelocity255 = map( motorTurnVelocityRaw, -9000, 9000, 0, 255 );
            }

            // Update & Run Specified Motors
            if (header.indexOf("GET /run") >= 0) {
              if( checkbox0State == 1 ){
                //Void updateMotor( 0, motorTurnVelocityRaw );
                Serial.println(motorTurnVelocity255);
                analogWrite(output23, motorTurnVelocity255);
              }
              if( checkbox1State == 1 ){
                //Void updateMotor( 1, motorTurnVelocityRaw );
                Serial.println(motorTurnVelocity255);
                analogWrite(output22, motorTurnVelocity255);
              }
            }

            /*
            Note for later: 
              Turn the slider into a textbox.
                Rules for textbox: only accept -9000 to 9000
                Only accept numbers
              Create multiple textboxes for each speed and have a 
              checkbox that changes from control all and some.
            */
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the brightness control sliders
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;} ");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;} ");
            client.println(".slider { width: 300px; } </style>");            
            // Web Page Heading
            client.println("<body><h1>ESP32-WROOM-32 Web Server</h1>");
            
            // Html Slider
            client.println("<p>motorTurnVelocityRaw:</p>");
            client.println("<p><input type=\"range\" min=\"-9000\" max=\"9000\" value=\"" + String(motorTurnVelocityRaw) + "\" class=\"slider\" id=\"brightness23\" onchange=\"updateBrightness(23)\"></p>");
            client.println("<script>function updateBrightness(pin) { var brightness = document.getElementById('brightness' + pin).value; window.location.href = '/setBrightness' + pin + '?value=' + brightness; }</script>");
            
            // Html Checkboxes for motors 0 -> 7
            // 0
            client.println("<p><input type=\"checkbox\" value=\"" + String(checkbox0State) + "\" id=\"checkbox0\" " + String(checkbox0State ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox0()\"> Motor 0</p>");
            client.println("<script>function togglecheckbox0() { var state = document.getElementById('checkbox0').checked ? 1 : 0; window.location.href = '/setcheckbox0?state=' + state; }</script>");
            // 1
            client.println("<p><input type=\"checkbox\" value=\"" + String(checkbox1State) + "\" id=\"checkbox1\" " + String(checkbox1State ? "checked" : "") + " style=\"display: inline-block;\" onchange=\"togglecheckbox1()\"> Motor 1</p>");
            client.println("<script>function togglecheckbox1() { var state = document.getElementById('checkbox1').checked ? 1 : 0; window.location.href = '/setcheckbox1?state=' + state; }</script>");

            // Execute Button
            client.println("<p><a href=\"/run\"><button class=\"button\">Run</button></a></p>");

            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}