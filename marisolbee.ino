/*
 * Marisol.bee
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// DNS
const byte DNS_PORT = 53;
DNSServer dnsServer;

// IP
IPAddress apIP(192, 168, 4, 1);
 
//SSID and Password to your ESP Access Point
const char* ssid = "MarisolBee";
const char* password = "12345678";

// web PASS
String pass = "1234";
bool passCorrecta = false;

// lastErr
String lastErr = "";

// Pin
int LED_Pin = 2; //onboard led
int LED5_Pin = 5; //GPIO5 - D1
 
ESP8266WebServer server(80); //Server on port 80
 
//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
  String htmlPage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'>";
  htmlPage += "<style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  font-size: 120%;} .red { background-color: #CC0000;} .green { background-color: #4CAF50;}</style>";
  htmlPage += "<title>LED Control</title></head><body><div id='main'><h2>Marisolbee</h2>";

  if ( passCorrecta ) { // Hacer un redirect por javascript
    htmlPage += "<form id='F1' action='ON'><input class='button green' type='submit' value='ON'></form><br>";
    htmlPage += "<form id='F2' action='OFF'><input class='button green' type='submit' value='OFF'></form><br>";
    htmlPage += "</div></body></html>";
  } else { // formulario password
    htmlPage += "<form id='F1' action='PASS'>Password:<br><input type='password' name='psw'><br>";
    htmlPage += "<input class='button' type='submit' value='PASS'></form><br>";
    htmlPage += "</html>";
    htmlPage += "\r\n";
  }
  server.send(200, "text/html", htmlPage);
}

void handlePass() {
  String htmlPage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'>";
  htmlPage += "<style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  font-size: 120%;} .red { background-color: #CC0000;} .green { background-color: #4CAF50;}</style>";
  htmlPage += "<title>LED Control</title></head><body><div id='main'><h2>Marisolbee</h2>";

  // parametros url
  for (uint8_t i = 0; i < server.args(); i++) {
    htmlPage += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
    if (server.argName(i) == "psw" && server.arg(i) == pass) {
      passCorrecta = true;
      htmlPage += "Clave correcta<br>"; // incluir redirección javascript
    } else {
      htmlPage += "Clave INCORRECTA. Intenta nuevamente<br>";
      htmlPage += "<a href='http:marisolbee.local'>marisolbee.local</a>";
    }
  }
  htmlPage += "</html>";
  htmlPage += "\r\n";

  server.send(200, "text/html", htmlPage);
}

void handleOn() {
  String htmlPage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'>";
  htmlPage += "<style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  font-size: 120%;} .red { background-color: #CC0000;} .green { background-color: #4CAF50;}</style>";
  htmlPage += "<title>LED Control</title></head><body><div id='main'><h2>Marisolbee</h2>";
 
  htmlPage += "<form id='F1' action='ON'><input class='button green' type='submit' value='ON'></form><br>";
  htmlPage += "<form id='F2' action='OFF'><input class='button green' type='submit' value='OFF'></form><br>";
  htmlPage += "</div></body></html>";

  digitalWrite(LED_Pin, HIGH);  
  digitalWrite(LED5_Pin, HIGH);  
        
  server.send(200, "text/html", htmlPage);
}

void handleOff() {
  String htmlPage = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'>";
  htmlPage += "<style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  font-size: 120%;} .red { background-color: #CC0000;} .green { background-color: #4CAF50;}</style>";
  htmlPage += "<title>LED Control</title></head><body><div id='main'><h2>Marisolbee</h2>";

  htmlPage += "<form id='F1' action='ON'><input class='button green' type='submit' value='ON'></form><br>";
  htmlPage += "<form id='F2' action='OFF'><input class='button green' type='submit' value='OFF'></form><br>";
  htmlPage += "</div></body></html>";

  digitalWrite(LED_Pin, LOW);  
  digitalWrite(LED5_Pin, LOW);  
        
  server.send(200, "text/html", htmlPage);
}
 
//===============================================================
//                  SETUP
//===============================================================
void setup(void){
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP);           //Only Access point
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));  
  WiFi.softAP(ssid, password);  //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  // DNS
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);

  // Setup the DNS server redirecting all the domains to the apIP
  // dnsServer.start(DNS_PORT, "*", apIP);
  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "marisolbee.local", apIP);

  pinMode(LED_Pin, OUTPUT);
  pinMode(LED5_Pin, OUTPUT);
     
  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/PASS", handlePass);
  server.on("/ON", handleOn);
  server.on("/OFF", handleOff);

  server.onNotFound(handleRoot); // When a client requests an unknown URI (i.e. something other than "/")

         
  server.begin();  //Start server
  Serial.println("HTTP server started");
}
//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  dnsServer.processNextRequest();  
  server.handleClient(); //Handle client requests
}
