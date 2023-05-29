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
// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BMP280 Forced Mode Test."));



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
    //humidity = bmp.readHumidity();
    
    // Convert the value to a char array
    // char humString[8];
    // dtostrf(humidity, 1, 2, humString);
    // Serial.print("Humidity: ");
    // Serial.println(humString);
    // client.publish("esp32/humidity", humString);
  }

  // // must call this to wake sensor up and get new measurement data
  // // it blocks until measurement is complete
  // if (bmp.takeForcedMeasurement()) {
  //   // can now print out the new measurements
  //   Serial.print(F("Temperature = "));
  //   Serial.print(bmp.readTemperature());
  //   Serial.println(" *C");

  //   Serial.print(F("Pressure = "));
  //   Serial.print(bmp.readPressure());
  //   Serial.println(" Pa");

  //   Serial.print(F("Approx altitude = "));
  //   Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  //   Serial.println(" m");

  //   Serial.println();
  //   delay(2000);
  // } else {
  //   Serial.println("Forced measurement failed!");
  // }


}
