#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ArduinoJson.h>

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

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from OUTPUT pin of sensor
  //rele:
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

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
      
  }
  if (doc["action_cmd"]=="off") {
      Serial.println("off...");
      digitalWrite(RELAY_PIN, LOW);
  }
}
if (doc["action"]=="measure_now") {
    Serial.println("measure_now...");
    StaticJsonDocument<256> docRoot;
    StaticJsonDocument<256> data;
    docRoot["sensor_guid"] = uniqueGuid;
    char outJSONData[128];
    data["pir"] = "true";
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