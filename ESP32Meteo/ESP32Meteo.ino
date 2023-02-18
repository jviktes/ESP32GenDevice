/**
   A simple Azure IoT example for sending telemetry to Iot Hub.
*/
#include "Arduino.h"
#include "UUID.h"

#include <WiFi.h>
#include "Esp32MQTTClient.h"
#include "DHT.h"
#define INTERVAL 10000
#define MESSAGE_MAX_LEN 1024
// Please input the SSID and password of WiFi
const char* ssid = "dlink";
const char* password = ".MoNitor2?";

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
static const char* connectionString = "HostName=vikhub.azure-devices.net;DeviceId=esp32Temperature;SharedAccessSignature=SharedAccessSignature sr=vikhub.azure-devices.net%2Fdevices%2Fesp32Temperature&sig=dKldWv8IT9zPKp5VNB1P%2B4htBG2ebxs3Zak3Zmb2Qp0%3D&se=23275368339";
//const char *messageData = "{\"deviceId\":\"esp32Temperature\", \"messageId\":%d, \"Temperature\":%f, \"Humidity\":%f, \"Light\":%d, \"TemperatureBMP\":%f,\"Pressure\":%f}";

const char *messageData = "{\"Id\":\"%s\", \"MessageData\":{\"IdMessage\":\"%s\",\"DataSet\":[{\"Name\":\"huminidy\",\"Value\":\"%s\"},{\"Name\":\"light\",\"Value\":\"%s\"},{\"Name\":\"pressure\",\"Value\":\"%s\"},{\"Name\":\"temperature\",\"Value\":\"%s\"},{\"Name\":\"temperatureBMP\",\"Value\":\"%s\"}]},\"DeviceId\":\"esp32Temperature\",\"DeviceData\":{\"nameDevice\":\"ESP32 meteostanice\",\"DeviceId\":\"esp32Temperature\",\"DataSet\":[{\"Name\":\"voltage\",\"Value\":\"%s\"}]},\"MessageDate\":\"%s\"}";

static bool hasIoTHub = false;
static bool hasWifi = false;
int messageCount = 1;
static bool messageSending = true;
static uint64_t send_interval_ms;

//DHT SENSOR:
#define pinDHT 13
#define typDHT11 DHT11
DHT temperatureSensor(pinDHT, typDHT11);

//LIGHT SENSOR:
const int pResistor = A0;

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending temperature and humidity data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending temperature and humidity data");
    messageSending = false;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}



void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");
  Serial.println(" > WiFi");
  Serial.println("Starting connecting WiFi.");

  delay(10);
  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    hasWifi = false;
  }
  hasWifi = true;

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(" > IoT Hub");
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString, true))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println("Start sending events.");
  randomSeed(analogRead(0));
  send_interval_ms = millis();

}

void loop() {
  if (hasWifi && hasIoTHub)
  {
    if (messageSending &&
        (int)(millis() - send_interval_ms) >= INTERVAL)
    {
      // Send teperature data
      char messagePayload[MESSAGE_MAX_LEN];
      float temperature = getTemperatureDHT();//(float)random(0,500)/10;
      float humidity  = getHuminidy();//(float)random(0, 1000)/10;
      int light = getLightIntensity();
      float temperatureBMP = getTemperatureBMP280();
      float pressure = getPressureBMP280();
      char * messageDate = {"2023-02-18T14:54:38.8732099+01:00"};
      float voltage = (float)random(0, 1000) / 10;

      UUID uuid;
      uint32_t seed1 = random(999999999);
      uint32_t seed2 = random(999999999);
      uuid.seed(seed1, seed2);
      uuid.generate();
    
      uuid.toCharArray();
      Serial.println(uuid);
 
      snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, uuid.toCharArray(),uuid.toCharArray(), String(humidity), String(light), String(pressure), String(temperature), String(temperatureBMP), String(voltage),messageDate);
      Serial.println(messagePayload);
      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_SendEventInstance(message);
      send_interval_ms = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
  }
  delay(10);
}

float getHuminidy() {
  float _vlhkost = 0;
  _vlhkost = temperatureSensor.readHumidity();
  if (isnan(_vlhkost)) {

    Serial.println("Chyba pri cteni vlhkosti z DHT senzoru!");
  }
  else {
    Serial.println("Vlhkost: " + String(_vlhkost) + " %");
  }

  return _vlhkost;
}

float getTemperatureDHT() {
  //Serial.println("DHT readTemperature");
  float _vnitrniTeplota = 0;
  _vnitrniTeplota = temperatureSensor.readTemperature();

  if (isnan(_vnitrniTeplota)) {

    Serial.println("Chyba pri cteni teploty z DHT senzoru!");
  }
  else {
    Serial.println("DHT teplota:  " + String(_vnitrniTeplota) + ".");
  }

  return _vnitrniTeplota;
}

int getLightIntensity() {

  int lightInt = analogRead(pResistor);
  Serial.println("LightIntensity:" + String(lightInt));
  return lightInt;
}

float getTemperatureBMP280() {
  float _venkovniTeplota = (float)random(0, 1000) / 10; //bmp.readTemperature();
  Serial.println("bmpTeplota:" + String(_venkovniTeplota));
  return  _venkovniTeplota;
}

float getPressureBMP280() {
  //Serial.println("Tlak:");
  float _venkovniTlak = (float)random(0, 1000) / 10; //(bmp.readPressure() / 100.00);
  Serial.println("Tlak:" + String(_venkovniTlak));
  return  _venkovniTlak;
}
