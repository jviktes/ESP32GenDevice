float MeasureProximity() {

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  return distanceCm;
}
int ReadMoisture() {
  int waterValue=0;
  digitalWrite(POWER_PIN, HIGH);  //  turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  waterValue = analogRead(MOISTURE_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   //  turn the sensor OFF

  Serial.print("The water sensor MOISTURE_PIN: ");
  Serial.println(waterValue);
  return waterValue;
}
int ReadWater() {
  int waterValue=0;
  digitalWrite(POWER_PIN, HIGH);  //  turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  waterValue = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   //  turn the sensor OFF

  Serial.print("The water sensor waterValue: ");
  Serial.println(waterValue);
  return waterValue;
}
float ReadTemperature () {
    float temperature = 0.00;
    if (bmpEnabled) {
      if (bmp.takeForcedMeasurement()) {
        temperature = bmp.readTemperature();
        Serial.print("Temperature: ");
        Serial.println(temperature);
      }
    }

    return temperature;
}
float ReadPressure () {
    float pressure=0.00;
    if (bmpEnabled) {
      pressure = bmp.readPressure();
      Serial.print("Pressure: ");
      Serial.println(pressure);
    }
    return pressure;
}

