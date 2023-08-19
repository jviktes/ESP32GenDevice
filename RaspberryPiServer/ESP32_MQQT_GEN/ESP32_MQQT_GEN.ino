#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>

#include <Adafruit_BMP280.h>

String uniqueGuid = "esp32_gen_1"; 

// Replace the next variables with your SSID/Password combination
const char* ssid = "dlink";
const char* password = ".MoNitor2?";


// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.0.136";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int PIN_TO_SENSOR = 19; // GIOP19 pin connected to OUTPUT pin of sensor
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin

//rele
const int RELAY_PIN = 16; // ESP32 pin GIOP16 connected to the IN pin of relay

//echo
const int trigPin = 5;
const int echoPin = 18;
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;

//doba po sobe jsouci stisky:
const int PERIOD_BETWEEN_ALARMS = 1000;
static unsigned long lastAlarmTime_btn_pin4 = 0;
static unsigned long lastAlarmTime_btn_pin0 = 0;
static unsigned long lastAlarmTime_btn_pin2 = 0;
// set pin numbers
const int buttonPin_btn_pin4 = 4;
const int buttonPin_btn_pin0 = 15;
const int buttonPin_btn_pin2 = 2;

#define SHORT_PRESS_TIME 500 // 500 milliseconds

// Variables will change:
int currentState_btn_pin4;     // the current reading from the input pin
int currentState_btn_pin0; 
int currentState_btn_pin2; 

Adafruit_BMP280 bmp; 


AsyncWebServer server(80);

void Scanner() {

    Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from OUTPUT pin of sensor
  //rele:
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  //echo:
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // initialize the pushbutton pin as an input
  pinMode(buttonPin_btn_pin4, INPUT);
  pinMode(buttonPin_btn_pin0, INPUT);
  pinMode(buttonPin_btn_pin2, INPUT);

  //Meteo:
  Serial.println(F("BMP280 Forced Mode Test."));

  Scanner();
  //if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }
    /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */



  //String responseBasic = "Hi! I am ESP32 on "+String(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.println("Hi!");
      Serial.println(uniqueGuid);
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", uniqueGuid.c_str());
      request->send(response);
  });

  server.on("/IP", HTTP_GET, [](AsyncWebServerRequest* request) {
      IPAddress ip = WiFi.localIP();
      Serial.println(ip);
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", WiFi.localIP().toString().c_str());
      request->send(response);
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();

}

void GenerateJsonData(StaticJsonDocument<256> data, char* outResJsonData ) {

  char outJSONDataInternal[128];

    Serial.println("GenerateJsonData...");
    StaticJsonDocument<256> docRoot;
    //StaticJsonDocument<256> data;
    docRoot["sensor_guid"] = uniqueGuid;
    
    docRoot["data"] = data;
    serializeJson(docRoot, outJSONDataInternal);

    //memcpy(outResJsonData, outJSONDataInternal, sizeof(outResJsonData));
    strncpy(outResJsonData, outJSONDataInternal, sizeof(outJSONDataInternal));

    Serial.println("serializeJson:");
    Serial.println(outResJsonData);
    //return outJSONData;
}

float MeasureProximity() {

// Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  return distanceCm;
}




void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();


StaticJsonDocument <256> doc;
deserializeJson(doc,message);
Serial.println("action:");
const char* action = doc["action"];
Serial.println(action);

// Serial.println("serializeJson:");
// char out[128];
// int b =serializeJson(doc, out);
// Serial.println(out);

if (doc["action"]=="set_rele1") {
  if (doc["action_cmd"]=="on") {
      Serial.println("on...");
      digitalWrite(RELAY_PIN, HIGH);

      StaticJsonDocument<256> data;
      data["action"] =doc["action"];
      data["action_cmd"] =doc["action_cmd"];
      data["result"]="OK";
      String _topic = uniqueGuid+"/output";
      char outJSONData[128];
      GenerateJsonData(data,outJSONData);
      client.publish(_topic.c_str(),outJSONData);      
  }
  if (doc["action_cmd"]=="off") {
      Serial.println("off...");
      digitalWrite(RELAY_PIN, LOW);

      StaticJsonDocument<256> data;
      data["action"] =doc["action"];
      data["action_cmd"] =doc["action_cmd"];
      data["result"]="OK";
      String _topic = uniqueGuid+"/output";
      char outJSONData[128];
      GenerateJsonData(data,outJSONData);
      client.publish(_topic.c_str(),outJSONData);
  }
}
if (doc["action"]=="measure_now") {
    Serial.println("measure_now...");
    StaticJsonDocument<256> data;
    data["pir"] = "true";
    data["temperature"]=125.47;
    data["huminidy"]=666;
    data["action"]="measure_now";
    data["proximity"]=MeasureProximity();
    String _topic = uniqueGuid+"/output";
    char outJSONData[128];
    GenerateJsonData(data,outJSONData);
    client.publish(_topic.c_str(),outJSONData);
}
if (doc["action"]=="list") {
    Serial.println("list of supported operation...");
    StaticJsonDocument<256> docRoot;
    StaticJsonDocument<256> data;
    docRoot["sensor_guid"] = uniqueGuid;
    char outJSONData[128];
    data["action"] = "true";
    data["temperature"]=125.47;
    data["huminidy"]=80;
    data["action"]="measure_now";
    docRoot["data"] = data;
    serializeJson(docRoot, outJSONData);

    String _topic = uniqueGuid+"/output";
    Serial.println("measure_now..._topic");
    Serial.println(_topic);
    client.publish(_topic.c_str(),outJSONData);
}

if (doc["action"]=="proximity") {
  
    StaticJsonDocument<256> data;
    data["action"]="proximity";
    data["proximity"]=MeasureProximity();
    String _topic = uniqueGuid+"/output";
    char outJSONData[128];
    GenerateJsonData(data,outJSONData);
    client.publish(_topic.c_str(),outJSONData);
}

if (doc["action"]=="meteo") {
  
    StaticJsonDocument<256> data;
    data["action"]="meteo";
    
    float temperature=0.00;
    if (bmp.takeForcedMeasurement()) {
      temperature = bmp.readTemperature();  
      Serial.println(temperature); 
      char tempString[8];
      dtostrf(temperature, 1, 2, tempString);
      data["temperature"]=tempString;
    }
    
    float pressure = bmp.readPressure();
    Serial.println(pressure);
    //Convert the value to a char array
    char pressureString[16];
    dtostrf(pressure, 1, 2, pressureString);
    Serial.print("Pressure: ");
    Serial.println(pressureString);

    data["pressure"]=pressureString;

    String _topic = uniqueGuid+"/output";
    char outJSONData[128];
    GenerateJsonData(data,outJSONData);
    client.publish(_topic.c_str(),outJSONData);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect 
    if (client.connect(uniqueGuid.c_str())) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32pir/output");
      String _topic=uniqueGuid+"/input";
      client.subscribe(_topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  StaticJsonDocument<256> docRoot;
  StaticJsonDocument<256> data;
  docRoot["sensor_guid"] = uniqueGuid;
  char outJSONData[128];

  //buttons:
  currentState_btn_pin4 = digitalRead(buttonPin_btn_pin4);
  currentState_btn_pin0 = digitalRead(buttonPin_btn_pin0);
  currentState_btn_pin2 = digitalRead(buttonPin_btn_pin2);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  if (currentState_btn_pin4 == HIGH) {

      if(millis() - lastAlarmTime_btn_pin4 >= PERIOD_BETWEEN_ALARMS){
          Serial.println("publish HIGH");
          StaticJsonDocument<256> data;
          data["button1"]="pressed"; //TODO
          String _topic = uniqueGuid+"/output";
          char outJSONData[128];
          GenerateJsonData(data,outJSONData);
          client.publish(_topic.c_str(),outJSONData);

          lastAlarmTime_btn_pin4=millis();
      }
  }

  if (currentState_btn_pin0 == HIGH) {

      if(millis() - lastAlarmTime_btn_pin0 >= PERIOD_BETWEEN_ALARMS){
          Serial.println("publish HIGH");
          StaticJsonDocument<256> data;
          data["button2"]="pressed"; //TODO
          String _topic = uniqueGuid+"/output";
          char outJSONData[128];
          GenerateJsonData(data,outJSONData);
          client.publish(_topic.c_str(),outJSONData);

          lastAlarmTime_btn_pin0=millis();
      }
  }

    if (currentState_btn_pin2 == HIGH) {

      if(millis() - lastAlarmTime_btn_pin2 >= PERIOD_BETWEEN_ALARMS){
          Serial.println("publish HIGH");
          StaticJsonDocument<256> data;
          data["button3"]="pressed"; //TODO
          String _topic = uniqueGuid+"/output";
          char outJSONData[128];
          GenerateJsonData(data,outJSONData);
          client.publish(_topic.c_str(),outJSONData);

          lastAlarmTime_btn_pin2=millis();
      }
  }


  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");

    data["pir"] = "true";
    data["temperature"]=125.47;
    data["huminidy"]=80;
    docRoot["data"] = data;
    
    serializeJson(docRoot, outJSONData);
    String _topic = uniqueGuid+"/output";
    client.publish(_topic.c_str(),outJSONData);
  }
  else
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");

    data["pir"] = "false";
    data["temperature"]=125.47;
    data["huminidy"]=80;
    docRoot["data"] = data;
    
    serializeJson(docRoot, outJSONData);
    String _topic = uniqueGuid+"/output";
    client.publish(_topic.c_str(),outJSONData);
  }

}



