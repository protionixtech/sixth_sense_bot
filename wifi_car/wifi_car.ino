/* Use the libraries to use WiFi & create a web server */
#include <WiFi.h>
#include <WebServer.h>

/* Put your SSID & Password */
const char* ssid = "CAP10 BOT";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1); 
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80); // Give the port ID

// Define motor pins
int M1P = 25;
int M1N = 33;
int M2P = 27;
int M2N = 26;

void setup() {
  Serial.begin(115200);

  // Define pin modes for motor
  pinMode(M1P, OUTPUT);
  pinMode(M1N, OUTPUT);
  pinMode(M2P, OUTPUT);
  pinMode(M2N, OUTPUT);

  // Create HotSpot
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // Define resources (URL endpoints)
  server.on("/", movebot);
  server.onNotFound(handle_NotFound);

  // Start the server now
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // Handle clients
  delay(10);
}

void move_forward() {
  // Configurations for moving the bot forward
  digitalWrite(M1P, HIGH);
  digitalWrite(M1N, LOW);
  digitalWrite(M2P, HIGH);
  digitalWrite(M2N, LOW);
  Serial.println("Going Forward");
}

void move_backward() {
  // Configurations for moving the bot backward
  digitalWrite(M1P, LOW);
  digitalWrite(M1N, HIGH);
  digitalWrite(M2P, LOW);
  digitalWrite(M2N, HIGH);
  Serial.println("Going Backward");
}

void move_left() {
  // Configurations for moving the bot left
  digitalWrite(M1P, HIGH);
  digitalWrite(M1N, LOW);
  digitalWrite(M2P, LOW);
  digitalWrite(M2N, HIGH);
  Serial.println("Rotating Left");
}

void move_right() {
  // Configurations for moving the bot right
  digitalWrite(M1P, LOW);
  digitalWrite(M1N, HIGH);
  digitalWrite(M2P, HIGH);
  digitalWrite(M2N, LOW);
  Serial.println("Rotating Right");
}

void stop_() {
  // Configurations to stop the bot
  digitalWrite(M1P, LOW);
  digitalWrite(M1N, LOW);
  digitalWrite(M2P, LOW);
  digitalWrite(M2N, LOW);
  Serial.println("Stopped");
}

void movebot() {
  /*
   * Handle the incoming request to server;
   * Chack for the movement key received in argument
   * and work accordingly.
   */
  String message = "s";
  for (int i = 0; i < server.args(); i++) {
    if(server.argName(i) == "movement"){
      message = server.arg(i);
    }
  }

  if(message == "s") { stop_(); }
  else if(message == "f") { move_forward(); }
  else if(message == "b") { move_backward(); }
  else if(message == "l") { move_left(); }
  else if(message == "r") { move_right(); }
  
  server.send(200, "text/html", sendHTML());
}

void handle_NotFound(){
  // Handle the page not found error
  server.send(404, "text/plain", "Not found");
}

String sendHTML() {
  String html_text = "<!DOCTYPE html>"
  "<html>"
    "<head><meta name=viewport content=\"width=device-width, initial-scale=1.0, user-scalable=no\">"
    "<title>WiFi RC Car Control</title>"
    "<style>"
      "html { "
        "font-family: Helvetica;"
        "display: inline-block;"
        "margin: 0px auto;"
        "text-align: center;"
        "height: 100%;"
      "}"
      "body{"
        "margin-top: 50px;"
        "height: 100%;"
      "}"
      "h1 {"
        "color: #444444;"
        "margin: 50px auto 30px;"
      "}"
      "h3 {"
        "color: #444444;"
        "margin-bottom: 50px;"
      "}"
      ".button {"
        "display: block;"
        "width: 80px;"
        "background-color: #3498db;"
        "border: none;"
        "color: white;"
        "padding: 30px 30px;"
        "text-decoration: none;"
        "font-size: 25px;"
        "margin: 0px auto 35px;"
        "cursor: pointer;"
        "border-radius: 4px;"
      "}"
      ".button-on {"
        "background-color: #3498db;"
      "}"
      ".button-on:active {"
        "background-color: #2980b9;"
      "}"
      ".button-off {"
        "background-color: #34495e;"
      "}"
      ".button-off:active {"
        "background-color: #2c3e50;"
      "}"
      "p {"
        "font-size: 14px;"
        "color: #888;"
        "margin-bottom: 10px;"
      "}"
      "table {"
        "width: 80%;"
        "margin-left: auto;"
        "margin-right: auto;"
      "}"
      "td {"
        "width: 33%;"
        "height: 33%;"
      "}"
    "</style>"
  "</head>"
  "<body>"
    "<h1>WiFi RC Car Control</h1>"
    "<h3>Made by Protionix</h3>"
    "<hr>"
    "<p>"
      "<table>"
        "<tr>"
          "<td></td>"
          "<td><button id=\"front\" onpointerdown=\"userAction('f')\" onpointerup=\"userAction('s')\" class=\"button button-off\"></button></td>"
          "<td></td>"
        "</tr>"
        "<tr>"
          "<td><button id=\"left\" onpointerdown=\"userAction('l')\" onpointerup=\"userAction('s')\" class=\"button button-off\"></button></td>"
          "<td></td>"
          "<td><button id=\"right\" onpointerdown=\"userAction('r')\" onpointerup=\"userAction('s')\" class=\"button button-off\"></button></td>"
        "</tr>"
        "<tr>"
          "<td></td>"
          "<td><button id=\"back\" onpointerdown=\"userAction('b')\" onpointerup=\"userAction('s')\" class=\"button button-off\"></button></td>"
          "<td></td>"
        "</tr>"
      "</table>"
    "</p>"
    "<script>"
      "const userAction = async (movement) => {"
        "var call_url = encodeURI('/?movement=' + movement);"
        "console.log(call_url);"
        "if(movement == 's'){"
          "document.getElementById(\"front\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"left\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"right\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"back\").classList.replace(\"button-on\", \"button-off\");"
        "}"
        "else if(movement == 'f'){"
          "document.getElementById(\"front\").classList.replace(\"button-off\", \"button-on\");"
          "document.getElementById(\"left\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"right\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"back\").classList.replace(\"button-on\", \"button-off\");"
        "}"
        "else if(movement == 'b'){"
          "document.getElementById(\"front\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"left\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"right\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"back\").classList.replace(\"button-off\", \"button-on\");"
        "}"
        "else if(movement == 'l'){"
          "document.getElementById(\"front\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"left\").classList.replace(\"button-off\", \"button-on\");"
          "document.getElementById(\"right\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"back\").classList.replace(\"button-on\", \"button-off\");"
        "}"
        "else if(movement == 'r'){"
          "document.getElementById(\"front\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"left\").classList.replace(\"button-on\", \"button-off\");"
          "document.getElementById(\"right\").classList.replace(\"button-off\", \"button-on\");"
          "document.getElementById(\"back\").classList.replace(\"button-on\", \"button-off\");"
        "}"
        "const response = await fetch(call_url);"
        "const myJson = await response.json();"
      "}"
    "</script>"
  "</body>"
"</html>";
return html_text;
}
