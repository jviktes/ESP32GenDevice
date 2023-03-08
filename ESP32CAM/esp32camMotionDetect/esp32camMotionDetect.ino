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

/**
 * Instantiate motion detector
 */
EloquentSurveillance::Motion motion;

#define FLASH_GPIO_NUM 4

// Please input the SSID and password of WiFi
const char* ssid = "dlink";
const char* password = ".MoNitor2?";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

char tempMessagePayload[22];

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
    camera.uxga();
    //camera.svga(); //uxga: pomale ukladani na sd kart? //svga()
    camera.highQuality();

    /**
     * See MotionDetectionExample for more details
     */
    motion.setMinChanges(0.1);
    motion.setMinPixelDiff(10);
    motion.setMinSizeDiff(0.05);
    motion.debounce(1000L);

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
    // while (!SPIFFS.begin(true))
    //     debug("ERROR", "Cannot init SPIFFS");

  //Serial.println("Starting SD Card");
 

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
}

/**
 *
 */
void loop() {
    /**
     * Try to capture a frame
     * If something goes wrong, print the error message
     */
    if (!camera.capture()) {
        debug("ERROR", camera.getErrorMessage());
        return;
    }

    /**
     * Look for motion.
     * In the `true` branch, you can handle a motion event.
     * In this case, we save the frame to disk
     */
    if (!motion.update())
        return;

    if (motion.detect()) {

      Serial.printf("Motion detected!");
      char * messageDate = getLocalTime();
      debug("INFO",messageDate);

                String filename = "/pictures/"+String(messageDate) +".jpg";//motion.getNextFilename("/pictures/capture_");

                // Path where new picture will be saved in SD Card
                String path = filename;
                fs::FS &fs = SD_MMC; 
                Serial.printf("Picture file name: %s\n", path.c_str());
                File file = fs.open(path.c_str(), FILE_WRITE);

                  if(!file){
                    Serial.println("Failed to open file in writing mode");
                  } 
                  else {
                    file.write(camera.getBuffer(), camera.getFileSize()); // payload (image), payload length
                    Serial.printf("Saved file to path: %s\n", path.c_str());
                  }

                file.close();
                    
                //delay(500);
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
