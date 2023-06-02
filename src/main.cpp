/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
*********/

// Import required libraries
#include <Arduino.h>
#include <ArduinoOTA.h>
// #include <ESP8266WiFi.h>
// #include <Hash.h>
// #include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "GrossoModo";
const char* password = "resnonverba";
// Lecture DHT11 possible sur les GPIO n°:
// 4,2,15,23,22,21,19,18,5
// 32,33,25,26,27,14,12,13
#define DHTPIN1 15     // Digital pin connected to the DHT sensor - Black
#define DHTPIN2 02     // Digital pin connected to the DHT sensor - White
#define DHTPIN3 05     // Digital pin connected to the DHT sensor - Red
#define PINOUT1 22
#define PINOUT2 18
#define PINOUT3 19
#define PINOUT4 21
// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);

// current temperature & humidity, updated in loop()
float t1 = 0.0;
float t2 = 0.0;
float t3 = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 250;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:left;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Temperature mini-serre</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Rouge</span> 
    <span id="temperature1">%TEMPERATURE1%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Blanc</span> 
    <span id="temperature2">%TEMPERATURE2%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Noir</span> 
    <span id="temperature3">%TEMPERATURE3%</span>
    <sup class="units">&deg;C</sup>
  </p>






</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      const myArray = this.responseText.split(",");
      document.getElementById("temperature1").innerHTML = myArray[0];
      document.getElementById("temperature2").innerHTML = myArray[1];
      document.getElementById("temperature3").innerHTML = myArray[2];
    }
  };
  xhttp.open("GET", "/temperatures", true);
  xhttp.send();
}, 250 ) ;


</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE1"){
    return String(t1);
  }
  else if(var == "TEMPERATURE2"){
    return String(t2);
  }
    else if(var == "TEMPERATURE3"){
    return String(t3);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Setup start!");

  dht1.begin();
  dht2.begin();
  dht3.begin();
  pinMode(PINOUT1,OUTPUT);
  pinMode(PINOUT2,OUTPUT);
  pinMode(PINOUT3,OUTPUT);
  pinMode(PINOUT4,OUTPUT);
  digitalWrite(PINOUT1,LOW);
  digitalWrite(PINOUT2,LOW);
  digitalWrite(PINOUT3,LOW);
  digitalWrite(PINOUT4,LOW);

  // digitalWrite(PINOUT,HIGH);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println(".");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperatures", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", (String(t1)+","+String(t2)+","+String(t3)).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
  digitalWrite(PINOUT1,HIGH);
  delay(1000);
  digitalWrite(PINOUT2,HIGH);
  delay(1000);
  digitalWrite(PINOUT3,HIGH);
  delay(1000);
  digitalWrite(PINOUT4,HIGH);
  delay(1000);
  digitalWrite(PINOUT1,LOW);
  digitalWrite(PINOUT2,LOW);
  digitalWrite(PINOUT3,LOW);
  digitalWrite(PINOUT4,LOW);


      ArduinoOTA.setHostname("ESP32");
      ArduinoOTA.onStart([]() {
        Serial.println("Start");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
      ArduinoOTA.begin();
}
 
void loop(){  
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT1 = dht1.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT2 = dht2.readTemperature();
    float newT3 = dht3.readTemperature();
    bool p1=digitalRead(PINOUT1);
    // digitalWrite(PINOUT1,!digitalRead(PINOUT1));
    // digitalWrite(PINOUT2,!digitalRead(PINOUT2));
    // digitalWrite(PINOUT3,!digitalRead(PINOUT3));
    // digitalWrite(PINOUT4,!digitalRead(PINOUT4));
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT1)) {
      Serial.println("Failed to read from DHT1 sensor!");
    }
    else {
      t1 = newT1;
      Serial.println(t1);
    }
    if (isnan(newT2)) {
      Serial.println("Failed to read from DHT2 sensor!");
    }
    else {
      t2 = newT2;
      Serial.println(t2);
    }
    if (isnan(newT3)) {
      Serial.println("Failed to read from DHT3 sensor!");
    }
    else {
      t3 = newT3;
      Serial.println(t3);
    }    
  }
}