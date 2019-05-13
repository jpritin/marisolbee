/*
 * Marisol.bee - Bicicleta eléctrica
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Default Values
// SSID and Password to your ESP Access Point
#define DEFAULT_SSID "MarisolBee";
#define DEFAULT_PASSWORD "12345678";
// web PASS
#define DEFAULT_PASS "1234";
// Time
#define DEFAULT_STARTTIME 0;
#define DEFAULT_INTERVAL 5000;

// DNS
const byte DNS_PORT = 53;
DNSServer dnsServer;

// IP
IPAddress apIP(192, 168, 4, 1);
 
//SSID and Password to your ESP Access Point
String ssid = DEFAULT_SSID;
String password = DEFAULT_PASSWORD;

// web PASS
String pass = DEFAULT_PASS;
bool passCorrecta = false;

// lastErr
String lastErr = "";

// Pin
int LED_Pin = 2; //onboard led
int Rele_Pin = 5; //GPIO5 - D1 - Relay

// Time
unsigned long StartTime = 0;
unsigned long interval = DEFAULT_INTERVAL;

// html
String htmlIni = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/><meta charset='utf-8'><style>body {font-size:140%;} #main {display: table; margin: auto;  padding: 0 10px 0 10px; } h2,{text-align:center; } .button { padding:10px 10px 10px 10px; width:100%;  font-size: 120%;} .red { background-color: #CC0000;} .green { background-color: #4CAF50;}</style><title>Marisol bee</title></head><body><div id='main'><h2>Marisol&nbsp;bee</h2>";

String htmlFin = "</div></body></html>";

String htmlRedirect = "<html><body><script type='text/javascript'>window.location.replace('http://marisolbee.local');</script></body></html>";
 
ESP8266WebServer server(80); //Server on port 80
 
//==============================================================
//     This rutine is exicuted when you open its IP in browser
//==============================================================
void handleRoot() {
  String htmlPage = htmlIni;

  if ( passCorrecta && (unsigned long)(millis()-StartTime) < interval) {   
    // Comprobamos Pin Rele
    if (digitalRead(Rele_Pin) == HIGH)
      htmlPage += "<form id='F1' action='on'><input class='button green' type='submit' value='Enciende'></form><br>";
    else
      htmlPage += "<form id='F1' action='off'><input class='button red' type='submit' value='Apaga'></form><br>";

    htmlPage += "Cambiar Clave<br>";
    htmlPage += "<a href='http://marisolbee.local/chgpassform'>Cambiar</a>";
      
    htmlPage += htmlFin;
  } else { // formulario password
    passCorrecta = false;
        
    htmlPage += "<form id='F1' action='pass' method='post' enctype='application/x-www-form-urlencoded'>Clave:<br>";
    htmlPage += "<input type='password' name='psw'><br>";
    htmlPage += "<input class='button' type='submit' value='Enviar'></form><br>";
    htmlPage += htmlFin;
  }
  server.send(200, "text/html", htmlPage);
}

void handlePass() {
  String htmlPage = htmlIni;

  passCorrecta  =  false;
  // parametros url
  for (uint8_t i = 0; i < server.args(); i++) {
    htmlPage += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
    if (server.argName(i) == "psw" && server.arg(i) == pass) {
      passCorrecta = true;
      StartTime = millis();      
    }
  }
  if(passCorrecta) { 
      htmlPage += "Clave correcta<br>"; // incluir redirección javascript
      htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      
  } else {
      htmlPage += "Clave INCORRECTA. Intenta nuevamente<br>";
      htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";
  }
  htmlPage += htmlFin;
  server.send(200, "text/html", htmlPage);
}

void handleRedirect() {
  server.send(200, "text/html", htmlRedirect);
}

void handleOn() {
  // Rele is off when output is LOW
  digitalWrite(LED_Pin, LOW);  
  digitalWrite(Rele_Pin, LOW);  

  // Ponemos nuevamente el contador de tiempo
  StartTime = millis();
        
  server.send(200, "text/html", htmlRedirect);
}

void handleOff() {
  // Rele is on when output is HIGH
  digitalWrite(LED_Pin, HIGH);  
  digitalWrite(Rele_Pin, HIGH);  

  // Ponemos nuevamente el contador de tiempo
  StartTime = millis();
        
  server.send(200, "text/html", htmlRedirect);
}

void handleCfg() {
  String htmlPage = htmlIni;

  // SSID and Password to your ESP Access Point
  // String ssid = DEFAULT_SSID;
  // String password = DEFAULT_PASSWORD;
  // web PASS
  // String pass = DEFAULT_PASS;
  // Time
  // unsigned long interval = DEFAULT_INTERVAL

  // Mostramos parámetros de configuración wifi
  htmlPage += "ssid : " + ssid + "<br>";
  htmlPage += "wifipass : " + password + "<br>"; 
  
  // Mostramos parámetros de configuración web pass
  htmlPage += "webpass : " + pass + "<br>"; 
  
  // Mostramos parámetros de configuración web pass
  htmlPage += "Intervalo : " + String(interval) + "<br>";   
  
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      

  htmlPage += htmlFin;

  server.send(200, "text/html", htmlPage);
}

void handleReset() {
  String htmlPage = htmlIni;

  // SSID and Password to your ESP Access Point
  ssid = DEFAULT_SSID;
  password = DEFAULT_PASSWORD;
  // web PASS
  pass = DEFAULT_PASS;
  // Time
  interval = DEFAULT_INTERVAL;
  saveCredentials();
  
  // Mostramos parámetros de configuración wifi
  htmlPage += "Valores de fábrica<br>";  
  htmlPage += "ssid : " + ssid + "<br>";
  htmlPage += "wifipass : " + password + "<br>"; 
  
  // Mostramos parámetros de configuración web pass
  htmlPage += "webpass : " + pass + "<br>"; 
 
  // Mostramos parámetros de configuración web pass
  htmlPage += "Intervalo : " + String(interval) + "<br>";   
  
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      

  htmlPage += htmlFin;

  server.send(200, "text/html", htmlPage);
}

void handleChgPassForm() {
  String htmlPage = htmlIni;
  htmlPage += "<form id='F1' action='chgpass' method='get'>";
  htmlPage += "Clave:<input name='pwdold' type='password' value=''><br>";
  htmlPage += "Nueva:<input name='pwdnew' type='password' value=''><br>";
  htmlPage += "Repita:<input name='pwdrep' type='password' value=''><br>";
  htmlPage += "<input class='button' type='submit' value='Enviar'></form><br>";

  htmlPage += "Cambiar Parámetros<br>";
  htmlPage += "<a href='http://marisolbee.local/chgparamform'>Cambiar</a><br>";
  
  htmlPage += "Volver<br>"; // incluir redirección javascript
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";   

  htmlPage += htmlFin;
  server.send(200, "text/html", htmlPage);
}

void handleChgPass() {
  String pwdold = "",pwdnew ="",pwdrep = "";
  String htmlPage = htmlIni;
  passCorrecta  =  false;

  // parametros formulario
  for (uint8_t i = 0; i < server.args(); i++) {
    htmlPage += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
    if (server.argName(i) == "pwdold") {
        pwdold = String(server.arg(i));
     }
    if (server.argName(i) == "pwdnew") {
        pwdnew = String(server.arg(i));
     }     
    if (server.argName(i) == "pwdrep") {
        pwdrep = String(server.arg(i));
     }
  }
  if(pwdold == pass && pwdnew == pwdrep) { 
      pass = pwdnew;
      saveCredentials();
      htmlPage += "Clave cambiada<br>"; // incluir redirección javascript
      htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      
  } else {
      htmlPage += "Clave INCORRECTA. Intenta nuevamente<br>";
      htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";
  }
  htmlPage += htmlFin;
  server.send(200, "text/html", htmlPage);
}


void handleChgParamForm() {
  String htmlPage = htmlIni;
  htmlPage += "<form id='F1' action='chgparam' method='get'>";
  htmlPage += "ssid:<input name='ssid' type='text' value='" + ssid+"'><br>";
  htmlPage += "wifi pass:<input name='password' type='text' '" + password + "'><br>";
  htmlPage += "intervalo:<input name='interval' type='text' '" + String(interval) + "'><br>";

  htmlPage += "<input class='button' type='submit' value='Enviar'></form><br>";

  htmlPage += "Cambiar Clave de acceso<br>";
  htmlPage += "<a href='http://marisolbee.local/chgpassform'>Cambiar clave</a><br>";

  htmlPage += "Volver<br>"; // incluir redirección javascript
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>"; 

  htmlPage += htmlFin;
  server.send(200, "text/html", htmlPage);
}

void handleChgParam() {
  String nssid, npassword;
  unsigned long ninterval;
  
  String htmlPage = htmlIni;
  passCorrecta  =  false;
  
  // parametros formulario
  for (uint8_t i = 0; i < server.args(); i++) {
    htmlPage += " " + server.argName(i) + ": " + server.arg(i) + "<br>";
    if (server.argName(i) == "ssid") {
        nssid = String(server.arg(i));
    }
    if (server.argName(i) == "password") {
        npassword = String(server.arg(i));
    }     
    if (server.argName(i) == "interval") {
        ninterval = String(server.arg(i)).toInt();
    }
  }
  ssid = nssid;
  password = npassword;
  interval = ninterval;
  saveCredentials();  
  
  htmlPage += "Parámetros cambiados<br>"; // incluir redirección
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      
  htmlPage += htmlFin;
  server.send(200, "text/html", htmlPage);
}

void handleSalva() {
  String htmlPage = htmlIni;

  saveCredentials();
  // Mostramos los valore previos
  // Mostramos parámetros de configuración wifi
  htmlPage += "Valores previos <br>";
  htmlPage += "ssid : " + ssid + "<br>";
  htmlPage += "wifipass : " + password + "<br>";  
  // Mostramos parámetros de configuración web pass
  htmlPage += "webpass : " + pass + "<br>"; 
  // Mostramos parámetros de configuración web pass
  htmlPage += "Intervalo : " + String(interval) + "<br>";   

  loadCredentials();
  // Mostramos los valore recuperados
  // Mostramos parámetros de configuración wifi
  htmlPage += "Valores previos <br>";
  htmlPage += "ssid : " + ssid + "<br>";
  htmlPage += "wifipass : " + password + "<br>"; 
  // Mostramos parámetros de configuración web pass
  htmlPage += "webpass : " + pass + "<br>"; 
  // Mostramos parámetros de configuración web pass
  htmlPage += "Intervalo : " + String(interval) + "<br>";

  
  htmlPage += "<a href='http://marisolbee.local'>marisolbee.local</a>";      
  htmlPage += htmlFin;

  server.send(200, "text/html", htmlPage);
}

/** Load credentials from EEPROM */
void loadCredentials() { 
  EEPROM.begin(64);
  // SSID and Password to your ESP Access Point
  // ssid = "MarisolBee";
  // password = "12345678";
  // web PASS
  // pass = "1234";
  // Time
  // interval = 50000;
  EEPROM.get(0, ssid);
  EEPROM.get(0 + sizeof(ssid), password);
  EEPROM.get(0 + sizeof(ssid) + 20, pass);
  EEPROM.get(0 + sizeof(ssid) + 20 + 20, interval);  

  EEPROM.end();
}

/** Store credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(64);
  // SSID and Password to your ESP Access Point
  // ssid = "MarisolBee";
  // password = "12345678";
  // web PASS
  // pass = "1234";
  // Time
  // interval = 50000;  
  EEPROM.put(0, ssid);
  EEPROM.put(0 + sizeof(ssid), password);
  EEPROM.put(0 + sizeof(ssid) + 20, pass);
  EEPROM.put(0 + sizeof(ssid) + 20 + 20, interval);

  EEPROM.commit();
  EEPROM.end();
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
  // dnsServer.setErrorReplyCode(DNSReplyCode::NoError);

  // Setup the DNS server redirecting all the domains to the apIP
  // dnsServer.start(DNS_PORT, "*", apIP);
  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "marisolbee.local", apIP);

  pinMode(LED_Pin, OUTPUT);
  pinMode(Rele_Pin, OUTPUT);

  // Set rele off
  // Rele is off when output is HIGH
  digitalWrite(LED_Pin, HIGH);  
  digitalWrite(Rele_Pin, HIGH); 
  
  server.on("/", handleRoot);      //Which routine to handle at root location
  // server.on("/pass",HTTP_POST, handlePass); 
  server.on("/pass", handlePass);   
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/cfg", handleCfg);
  server.on("/rst", handleReset);  
  server.on("/chgpassform", handleChgPassForm);  
  server.on("/chgpass", handleChgPass);
  server.on("/chgparamform", handleChgParamForm);  
  server.on("/chgparam", handleChgParam);  
  server.on("/salva", handleSalva);   
  
  server.onNotFound(handleRedirect); // When a client requests an unknown URI (i.e. something other than "/")

  server.begin();  //Start server
  Serial.println("HTTP server started");

  // iniciamos contador tiempo
  StartTime = millis();
}

//===============================================================
//                     LOOP
//===============================================================
void loop(void){
  dnsServer.processNextRequest();  
  server.handleClient(); //Handle client requests
}
