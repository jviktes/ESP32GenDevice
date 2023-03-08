/**
 * Save motion capture to SPIFFS filesysten
 * @author  EloquentArduino <support@eloquentarduino.com>
 */
 // turn on debug messages
#define INFO
#include <SPIFFS.h>
#include "EloquentSurveillance.h"

#include "SD_MMC.h"            // SD Card ESP32

/**
 * Instantiate motion detector
 */
EloquentSurveillance::Motion motion;

#define FLASH_GPIO_NUM 4
/**
 *
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
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
                
                String filename = motion.getNextFilename("/pictures/capture_");

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