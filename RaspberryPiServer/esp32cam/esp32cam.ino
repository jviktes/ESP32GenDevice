/**
 * Save motion capture to SPIFFS filesysten
 * @author  EloquentArduino <support@eloquentarduino.com>
 */
 // turn on debug messages
#define INFO
#include <SPIFFS.h>
#include "EloquentSurveillance.h"

#include "SD_MMC.h"            // SD Card ESP32
#include "time.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.0.136";

AsyncWebServer server(80);

//OTA update:
#include <AsyncTCP.h>
#include <AsyncElegantOTA.h>

/**
 * Instantiate motion detector
 */
//EloquentSurveillance::Motion motion;

// Please input the SSID and password of WiFi
const char* ssid = "dlink";
const char* password = ".MoNitor2?";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

char tempMessagePayload[22];

String serverName = "192.168.0.136";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
String serverPath = "/upload";     // The default serverPath should be upload.php
const int serverPort = 4000;

WiFiClient client;
PubSubClient pubClient(client);



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

  //init time:
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
    debug("INFO", "Init");

    /**
     * See CameraCaptureExample for more details
     */
    camera.aithinker();
    //camera.uxga();
    camera.svga(); //uxga: pomale ukladani na sd kart? //svga()
    camera.highQuality();

    /**
     * See MotionDetectionExample for more details
     */
    // motion.setMinChanges(0.1);
    // motion.setMinPixelDiff(10);
    // motion.setMinSizeDiff(0.05);
    // motion.debounce(1000L);

    /**
     * Initialize the camera
     * If something goes wrong, print the error message
     */
    while (!camera.begin())
        debug("ERROR", camera.getErrorMessage());

    /**
     * Initialize the filesystem
     * If something goes wrong, print an error message
     */
    while (!SPIFFS.begin(true))
         debug("ERROR", "Cannot init SPIFFS");

  Serial.println("Starting SD Card");
 

  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
    debug("INFO","usedBytes:");
    debug("INFO",SPIFFS.usedBytes());
    debug("SUCCESS", "Camera OK");

     //String responseBasic = "Hi! I am ESP32 on "+String(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.print("Hi! I am ESP32-CAM ");
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am ESP32-Meteostanice-v1");
      request->send(response);
  });
  server.on("/take", HTTP_GET, [](AsyncWebServerRequest* request) {
      Serial.print("Hi! I am ESP32-CAM take");
      takePicture();
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html; charset=utf-8", "Hi! I am ESP32-CAM-take picture");
      request->send(response);
  });
  


  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();

  Serial.println("server.begin OK");

  char * messageDate = getLocalTime();
  Serial.println("messageDate:");
  Serial.println(messageDate);

  pubClient.setServer(mqtt_server, 1883);
  pubClient.setCallback(callback);

  


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

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32cam/photo") {
    Serial.print("Changing output to ");
    if(messageTemp == "take"){
      Serial.println("take");
      takePicture();
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
      pubClient.publish("esp32cam/photo","esp32cam start!");
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

char *getLocalTime() {
  struct tm timeinfo;
  const char *dateData = "%s-%s-%s_%s_%s_%s";
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return tempMessagePayload;
  }

  snprintf(tempMessagePayload, 22, dateData, String(timeinfo.tm_year + 1900), String(timeinfo.tm_mon+1), String(timeinfo.tm_mday),String(timeinfo.tm_hour),String(timeinfo.tm_min),String(timeinfo.tm_sec));
  return tempMessagePayload;
}

void takePicture() {
      Serial.println("takePicture");
      
    if (!camera.capture()) {
        debug("ERROR", camera.getErrorMessage());
        return;
    }

      char * messageDate = getLocalTime();
      Serial.println("messageDate:");
      debug("INFO",messageDate);

                //Path where new picture will be saved in SD Card
                String path = "/pictures/"+String(messageDate) +".jpg";//motion.getNextFilename("/pictures/capture_");

                fs::FS &fs = SD_MMC; 
                Serial.printf("Picture file name: %s\n", path.c_str());
                File file = fs.open(path.c_str(), FILE_WRITE);
                  if(!file){
                    Serial.println("Failed to open file in writing mode");
                  } 
                  else {
                    file.write(camera.getBuffer(), camera.getFileSize()); // payload (image), payload length
                    Serial.printf("Saved file to path: %s\n", path.c_str());
                    //delay(1000);
                  }
                file.close();
                uploadPhoto(path,messageDate);
                delay(1000);
}

String uploadPhoto(String filePath,String fileName) {

  String getAll;
  String getBody;

  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\""+fileName+".jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    fs::FS &fs = SD_MMC; 
    Serial.printf("Picture file uploading: %s\n", filePath.c_str());
    File file = fs.open(filePath.c_str(), FILE_READ);

    uint32_t imageLen = file.size();
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
    Serial.println("file size imageLen:");
    Serial.println(imageLen);

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);
  
    while (file.available()) {
      client.write(file.read());
    }

    file.close();
    client.print(tail);
    client.stop();
    Serial.println(getBody);

  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}

