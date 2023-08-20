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

const int messageSize = 256;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int PIN_TO_SENSOR = 19;  // GIOP19 pin connected to OUTPUT pin of sensor
int pinStateCurrent = LOW;     // current state of pin
int pinStatePrevious = LOW;    // previous state of pin

//rele
const int RELAY_PIN = 16;  // ESP32 pin GIOP16 connected to the IN pin of relay

//echo distance
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

//I2C
int currentState_btn_pin_20[8];
long lastAlarmTime_btn_pin_20[8];

int currentState_btn_pin_24[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int lastState_btn_pin_24[8]= { 0, 0, 0, 0, 0, 0, 0, 0 };

int enabled_button_mask_20[8] = { 1, 1, 1, 1, 0, 0, 0, 0 };
int enabled_button_mask_24[8]= { 1, 0, 0, 0, 0, 0, 0, 0 };
#define addr_stickers 0x21
#define addr_buttons 0x20

//water:
#define POWER_PIN  17 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define SIGNAL_PIN 34 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin

//moisture
#define MOISTURE_PIN 35 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin

Adafruit_BMP280 bmp;
AsyncWebServer server(80);


void PressedButtonI2C(int pin) {

  if (currentState_btn_pin_20[pin] == HIGH) {

    if (millis() - lastAlarmTime_btn_pin_20[pin] >= PERIOD_BETWEEN_ALARMS) {
      Serial.println("publish HIGH");
      StaticJsonDocument<messageSize> data;
      data["button_pin_20"] = "pressed";
      data["pin"] = pin;
      data["i2c_address"] = addr_buttons;
      String _topic = uniqueGuid + "/output";
      char outJSONData[messageSize];
      GenerateJsonData(data, outJSONData);
      client.publish(_topic.c_str(), outJSONData);
      lastAlarmTime_btn_pin_20[pin] = millis();
    }
  }
}

void PressedStickerI2C(int pin) {
  
  if (currentState_btn_pin_24[pin] != lastState_btn_pin_24[pin]) {

    //if (millis() - lastAlarmTime_btn_pin_24[pin] >= PERIOD_BETWEEN_ALARMS) {
      Serial.println("PressedStickerI2C");
      StaticJsonDocument<messageSize> data;
      data["sticker_pin_24"] = "pressed";
      data["pin"] = pin;
      data["i2c_address"] = addr_stickers;
      String _topic = uniqueGuid + "/output";
      char outJSONData[messageSize];
      GenerateJsonData(data, outJSONData);
      client.publish(_topic.c_str(), outJSONData);
      //lastAlarmTime_btn_pin_20[pin] = millis();
      lastState_btn_pin_24[pin]=currentState_btn_pin_24[pin];
    //}
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(PIN_TO_SENSOR, INPUT);  // set ESP32 pin to input mode to read value from OUTPUT pin of sensor
  //rele:
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  //echo:
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input

  //Meteo:
  Serial.println(F("BMP280 Forced Mode Test."));

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

  WriteTo(addr_stickers,0); //reset LED atd.
  WriteTo(addr_buttons,0); //reset LED atd.
  //water:
  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // low  turn the sensor OFF

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

  AsyncElegantOTA.begin(&server);  // Start ElegantOTA
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

  StaticJsonDocument<messageSize> doc;
  deserializeJson(doc, message);

  const char* action = doc["action"];
  Serial.println("action:");
  Serial.println(action);

  // Serial.println("serializeJson:");
  // char out[messageSize];
  // int b =serializeJson(doc, out);
  // Serial.println(out);

  if (doc["action"] == "set_rele1") {
    if (doc["action_cmd"] == "on") {
      Serial.println("on...");
      digitalWrite(RELAY_PIN, HIGH);

      StaticJsonDocument<messageSize> data;
      data["action"] = doc["action"];
      data["action_cmd"] = doc["action_cmd"];
      data["result"] = "OK";
      String _topic = uniqueGuid + "/output";
      char outJSONData[messageSize];
      GenerateJsonData(data, outJSONData);
      client.publish(_topic.c_str(), outJSONData);
    }
    if (doc["action_cmd"] == "off") {
      Serial.println("off...");
      digitalWrite(RELAY_PIN, LOW);

      StaticJsonDocument<messageSize> data;
      data["action"] = doc["action"];
      data["action_cmd"] = doc["action_cmd"];
      data["result"] = "OK";
      String _topic = uniqueGuid + "/output";
      char outJSONData[messageSize];
      GenerateJsonData(data, outJSONData);
      client.publish(_topic.c_str(), outJSONData);
    }
  }
  if (doc["action"] == "measure_now") {
    Serial.println("measure_now...");
    StaticJsonDocument<messageSize> data;
    data["pir"] = "true";
    data["temperature"] = 125.47;
    data["huminidy"] = 666;
    data["action"] = "measure_now";
    data["proximity"] = MeasureProximity();
    data["water"] =ReadWater();
    data["moisture"] = ReadMoisture();

    String _topic = uniqueGuid + "/output";
    char outJSONData[messageSize];
    GenerateJsonData(data, outJSONData);
    client.publish(_topic.c_str(), outJSONData);
  }
  if (doc["action"] == "list") {
    Serial.println("list of supported operation...");
    StaticJsonDocument<messageSize> docRoot;
    StaticJsonDocument<messageSize> data;
    docRoot["sensor_guid"] = uniqueGuid;
    char outJSONData[messageSize];
    data["action"] = "true";
    data["temperature"] = 125.47;
    data["huminidy"] = 80;
    data["action"] = "measure_now";
    docRoot["data"] = data;
    serializeJson(docRoot, outJSONData);

    String _topic = uniqueGuid + "/output";
    Serial.println("measure_now..._topic");
    Serial.println(_topic);
    client.publish(_topic.c_str(), outJSONData);
  }

  if (doc["action"] == "proximity") {

    StaticJsonDocument<messageSize> data;
    data["action"] = "proximity";
    data["proximity"] = MeasureProximity();
    String _topic = uniqueGuid + "/output";
    char outJSONData[messageSize];
    GenerateJsonData(data, outJSONData);
    client.publish(_topic.c_str(), outJSONData);
  }

  if (doc["action"] == "meteo") {

    StaticJsonDocument<messageSize> data;
    data["action"] = "meteo";

    float temperature = 0.00;
    if (bmp.takeForcedMeasurement()) {
      temperature = bmp.readTemperature();
      Serial.println(temperature);
      char tempString[8];
      dtostrf(temperature, 1, 2, tempString);
      data["temperature"] = tempString;
    }

    float pressure = bmp.readPressure();
    Serial.println(pressure);
    //Convert the value to a char array
    char pressureString[16];
    dtostrf(pressure, 1, 2, pressureString);
    Serial.print("Pressure: ");
    Serial.println(pressureString);

    data["pressure"] = pressureString;

    String _topic = uniqueGuid + "/output";
    char outJSONData[messageSize];
    GenerateJsonData(data, outJSONData);
    client.publish(_topic.c_str(), outJSONData);
  }

  if (doc["action"] == "leds") {

    StaticJsonDocument<messageSize> data;
    data["action"] = "leds";
    data["action_cmd"] = doc["action_cmd"];

    String _data = doc["action_cmd"];
    //129 = 10000001
    //126 = 01111110 

    Serial.println(_data);

    WriteTo(addr_buttons, _data.toInt());

    String _topic = uniqueGuid + "/output";
    char outJSONData[messageSize];
    GenerateJsonData(data, outJSONData);
    client.publish(_topic.c_str(), outJSONData);
  }

  if (doc["action"] == "scanner") {

    StaticJsonDocument<messageSize> data;
    data["action"] = "scanner";
    data["action_cmd"] = doc["action_cmd"];

    int i2cReport [20];
    Scanner(i2cReport);

    char outJSONDataInternalScanner[50];
    StaticJsonDocument<50> doc;
    copyArray(i2cReport, doc);
    serializeJson(doc, outJSONDataInternalScanner);

    data["scanner_data"] = outJSONDataInternalScanner;

    String _topic = uniqueGuid + "/output";
    char outJSONData[messageSize];
    GenerateJsonData(data, outJSONData);
    client.publish(_topic.c_str(), outJSONData);
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
      String _topic = uniqueGuid + "/input";
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

  pinStatePrevious = pinStateCurrent;            // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);  // read new state

  StaticJsonDocument<messageSize> docRoot;
  StaticJsonDocument<messageSize> data;
  docRoot["sensor_guid"] = uniqueGuid;
  char outJSONData[messageSize];

  //PIR:
  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {  // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");

    data["pir"] = "true";
    data["temperature"] = 125.47;
    data["huminidy"] = 80;
    docRoot["data"] = data;

    serializeJson(docRoot, outJSONData);
    String _topic = uniqueGuid + "/output";
    client.publish(_topic.c_str(), outJSONData);
  } else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {  // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");

    data["pir"] = "false";
    data["temperature"] = 125.47;
    data["huminidy"] = 80;
    docRoot["data"] = data;

    serializeJson(docRoot, outJSONData);
    String _topic = uniqueGuid + "/output";
    client.publish(_topic.c_str(), outJSONData);
  }

  //I2C bus - buttons:
  char outJSONDataI2C[8] = "0000000";
  ReadFrom2(addr_buttons, outJSONDataI2C);

  //read from occupied pins: if = 1 => nothing, if = 0 then pressed
  //evaluation values:
  for (byte i = 0; i < 8; i++) {
    if (enabled_button_mask_20[i] == 1 && String(outJSONDataI2C[i]) == "0") {
      currentState_btn_pin_20[i] = HIGH;
      PressedButtonI2C(i);
    }
  }

  //I2C bus - stickers:
  char outJSONDataI2CStickers[8] = "0000000";
  ReadFrom2(addr_stickers, outJSONDataI2CStickers);

  //read from occupied pins: if = 1 => nothing, if = 0 then pressed
  //evaluation values:
  for (byte i = 0; i < 8; i++) {
    if (enabled_button_mask_24[i] == 1) {
      currentState_btn_pin_24[i] = outJSONDataI2CStickers[i];
      PressedStickerI2C(i);
    }
  }


}
