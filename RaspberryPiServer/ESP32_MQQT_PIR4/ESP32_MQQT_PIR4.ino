#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "dlink";
const char* password = ".MoNitor2?";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
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
      Serial.print("Hi! I am ESP32-PIR1 ");
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am ESP32-PIR1");
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

  if (String(topic) == "esp32/light") { //TODO
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(RELAY_PIN, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
       digitalWrite(RELAY_PIN, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client_PIR")) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32pir/output");
      client.subscribe("esp32/light");
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
  
  //toto je pro casove urceni - 1x za 5s neco udelat:
  // long now = millis();
  // if (now - lastMsg > 5000) {
  //   lastMsg = now;
    
  //   // Convert the value to a char array
  //   char tempString[8];
  //   dtostrf(temperature, 1, 2, tempString);
  //   Serial.print("Temperature: ");
  //   Serial.println(tempString);
  //   client.publish("esp32/temperature", tempString);
  // }

  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");
    // TODO: turn on alarm, light or activate a device ... here
    client.publish("esp32pir/pir","Motion detected!");
  }
  else
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");
    client.publish("esp32pir/pir","Motion stopped!");
    // TODO: turn off alarm, light or deactivate a device ... here
  }

}