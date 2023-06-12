/**
 * Save motion capture to SPIFFS filesysten
 * @author  EloquentArduino <support@eloquentarduino.com>
 */

 // turn on debug messages
#define INFO

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.0.136";

// Please input the SSID and password of WiFi
const char* ssid = "dlink";
const char* password = ".MoNitor2?";

String serverName = "192.168.0.136";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
WiFiClient wifiClient;
PubSubClient pubClient(wifiClient);
AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    delay(3000);

    WiFi.mode(WIFI_AP);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("server.begin OK");

  pubClient.setServer(mqtt_server, 1883);
  pubClient.setCallback(callback);

     //String responseBasic = "Hi! I am ESP32 on "+String(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.print("Hi! I am ESP32-CAM ");
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am ESP32-CAM-v1");
      request->send(response);
  });
  server.on("/take", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.print("Hi! I am ESP32-CAM take");
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am ESP32-CAM-take picture");
      request->send(response);
  });
  
  server.begin();

}

/**
 *
 */
void loop() {
    if (!pubClient.connected()) {
    reconnect();
  }
  pubClient.loop();
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

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32cam/photo") {
    Serial.print("received order:");
    if(messageTemp == "take"){
      Serial.println("take");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!pubClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubClient.connect("ESP32CAMClient")) {
      Serial.println("connected");
      // Subscribe
      pubClient.subscribe("esp32cam/photo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


