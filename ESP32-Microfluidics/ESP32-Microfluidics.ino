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

int rawbrightness22 = 0;
int rawbrightness23 = 0;

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

            
            // turns the GPIOs on and off
            if (header.indexOf("GET /setBrightness23?value=") >= 0) {
              rawbrightness23 = header.substring(header.indexOf("value=") + 6).toInt();
              Serial.print("Setting brightness of GPIO 23 to: ");
              int mappedbrightness23 = map( rawbrightness23, -9000, 9000, 0, 255 );
              Serial.println(mappedbrightness23);
              analogWrite(output23, mappedbrightness23);
            }
            else if (header.indexOf("GET /setBrightness22?value=") >= 0) {
              rawbrightness22 = header.substring(header.indexOf("value=") + 6).toInt();
              Serial.print("Setting brightness of GPIO 22 to: ");
              int mappedbrightness22 = map( rawbrightness22, -9000, 9000, 0, 255 );
              Serial.println(mappedbrightness22);
              analogWrite(output22, mappedbrightness22);
            }
            
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
            
            // Display current brightness slider for GPIO 23
            client.println("<p>GPIO 23 Brightness:</p>");
            client.println("<p><input type=\"range\" min=\"-9000\" max=\"9000\" value=\"" + String(rawbrightness23) + "\" class=\"slider\" id=\"brightness23\" onchange=\"updateBrightness(23)\"></p>");
            client.println("<script>function updateBrightness(pin) { var brightness = document.getElementById('brightness' + pin).value; window.location.href = '/setBrightness' + pin + '?value=' + brightness; }</script>");
            
            // Display current brightness slider for GPIO 22
            client.println("<p>GPIO 22 Brightness:</p>");
            client.println("<p><input type=\"range\" min=\"-9000\" max=\"9000\" value=\"" + String(rawbrightness22) + "\" class=\"slider\" id=\"brightness22\" onchange=\"updateBrightness(22)\"></p>");
            
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