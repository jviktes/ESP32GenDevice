// Complete Instructions: https://RandomNerdTutorials.com/esp32-digital-inputs-outputs-arduino/

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
//doba po sobe jsouci stisky:
const int PERIOD_BETWEEN_ALARMS = 1000;
static unsigned long lastAlarmTime = 0;
//rele
const int RELAY_PIN = 16; // ESP32 pin GIOP16 connected to the IN pin of relay

// set pin numbers
const int buttonPin = 4;  // the number of the pushbutton pin
const int ledPin =  5;    // the number of the LED pin

// variable for storing the pushbutton status 

#define SHORT_PRESS_TIME 500 // 500 milliseconds

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  // initialize the pushbutton pin as an input
  pinMode(buttonPin, INPUT);
  // initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.print("Hi! I am esp32room1 ");
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am esp32room1");
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

  if (String(topic) == "esp32room1/light") { //TODO
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
    if (client.connect("esp32room1")) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32pir/output");
      client.subscribe("esp32room1");
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
  
  // read the state of the switch/button:
  currentState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH
  if (currentState == HIGH) {
    // turn LED on
    //digitalWrite(ledPin, HIGH);
      //Serial.println("HIGH");
      if(millis() - lastAlarmTime >= PERIOD_BETWEEN_ALARMS){
          Serial.println("publish HIGH");
          client.publish("esp32room1/button","HIGH");
          lastAlarmTime=millis();
      }
  } else {
    // turn LED off
    //digitalWrite(ledPin, LOW);
    //Serial.println("LOW");
  }
}