/***************************************************************************
  This is a library for the BMP280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; 

//DHT SENSOR:
#include "DHT.h"
#define pinDHT 13
#define typDHT11 DHT11
DHT temperatureSensor(pinDHT, typDHT11);

//LIGHT SENSOR:
const int pResistor = 34;

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

float temperature = 0;
float humidity = 0;
float pressure = 0;
float temperatureDHT = 0;
// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BMP280 Forced Mode Test."));


  temperatureSensor.begin(); // initialize the DHT sensor

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

  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

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

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
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
//Serial.print("Loop");
    if (!client.connected()) {
    reconnect();
  }
  client.loop();



  long now = millis();

// Serial.println("lastMsg");
// Serial.println(lastMsg);
// Serial.println("now");
// Serial.println(now);

  //kazdych 15 minut
  //TODO co configu..
  if (now - lastMsg > 1000) {
    lastMsg = now;
    //Serial.println("lastMsg");
    //Serial.println(lastMsg);
    // Temperature in Celsius
    if (bmp.takeForcedMeasurement()) {
    temperature = bmp.readTemperature();  
    Serial.println(temperature); 
    // Uncomment the next line to set temperature in Fahrenheit 
    // (and comment the previous temperature line)
    //temperature = 1.8 * bmp.readTemperature() + 32; // Temperature in Fahrenheit
    
    // Convert the value to a char array
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("esp32/temperature", tempString);

    pressure = bmp.readPressure();
    Serial.println(pressure);
    //Convert the value to a char array
    char pressureString[16];
    dtostrf(pressure, 1, 2, pressureString);
    Serial.print("Pressure: ");
    Serial.println(pressureString);
    client.publish("esp32/pressure", pressureString);

    }
    else {
      Serial.println("problem");
    }
    humidity = getHuminidy();
    
    //Convert the value to a char array
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("esp32/humidity", humString);

    temperatureDHT= getTemperatureDHT();
    char temperatureDHTString[8];
    dtostrf(temperatureDHT, 1, 2, temperatureDHTString);
    Serial.print("TemperatureDHT: ");
    Serial.println(temperatureDHTString);
    client.publish("esp32/temperatureDHT", temperatureDHTString);

  }

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
