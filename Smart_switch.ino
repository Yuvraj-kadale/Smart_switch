/*********
 "SMART SWITCH"
 This project is made by Yuvraj Kadale,
 As a internship project at Blackalphaa Software Solutions
*********/

#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"

#define RELAY_NO    true


#define T_relays  5 // For 5 relays


int relayGPIOs[T_relays] = {5, 4, 14, 12, 13};


const char* ssid = "";
const char* password = "";

const char* parameter_ip_1 = "relay";  
const char* parameter_ip_2 = "state";


AsyncWebServer server(80);

//Starting of Webpage//
const char index_html[] PROGMEM = R"rawliteral( 
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Blackalphaa Software Solutions</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?relay="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?relay="+element.id+"&state=0", true); }
  xhr.send();
}</script>
</body>
</html>
)rawliteral";


String processor(const String& var){
  
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    for(int i=1; i<=T_relays; i++){
      String relayStateValue = relayState(i);
      buttons+= "<h4>Switch no:" + String(i) + " - GPIO " + relayGPIOs[i-1] + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(i) + "\" "+ relayStateValue +"><span class=\"slider\"></span></label>";
    }
    return buttons;
  }
  return String();
}

String relayState(int numRelay){
  if(RELAY_NO){
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "";
    }
    else {
      return "checked";
    }
  }
  else {
    if(digitalRead(relayGPIOs[numRelay-1])){
      return "checked";
    }
    else {
      return "";
    }
  }
  return "";
}

void setup(){
  
  Serial.begin(115200);

  for(int i=1; i<=T_relays; i++){
    pinMode(relayGPIOs[i-1], OUTPUT);
    if(RELAY_NO){
      digitalWrite(relayGPIOs[i-1], HIGH);
    }
    else{
      digitalWrite(relayGPIOs[i-1], LOW);
    }
  }
  
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  
  Serial.println(WiFi.localIP());

  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {   // Sends GET request
    String inputMessage;
    String inputParam;
    String inputMessage2;
    String inputParam2;
     
    if (request->hasParam(parameter_ip_1) & request->hasParam(parameter_ip_2)) {
      inputMessage = request->getParam(parameter_ip_1)->value();
      inputParam = parameter_ip_1;
      inputMessage2 = request->getParam(parameter_ip_2)->value();
      inputParam2 = parameter_ip_2;
      if(RELAY_NO){
        Serial.print("NO");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
      }
      else{
        Serial.print("NC");
        digitalWrite(relayGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
      }
    }
    else {
      inputMessage = "No information sent";
      inputParam = "none";
    }
    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  // Start server
  server.begin();
}
  
void loop() {

}
