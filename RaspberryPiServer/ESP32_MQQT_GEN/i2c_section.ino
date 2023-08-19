void ReadFrom2(int addr, char* i2cdata) {
  Wire.requestFrom(addr, 1);
  if (Wire.available()) {
    //Serial.println("Wire.available()");
    byte readExpander = 255 - Wire.read();
    //String readed;
    for (byte i = 0; i < 8; i++) {
      //i2cdata+= String(bitRead(readExpander,i));
      i2cdata[i] = bitRead(readExpander, i) + 48;
    }
    // Serial.println("i2cdata:");
    // Serial.println(i2cdata);
  }
}

void WriteTo(int val) {
  Wire.beginTransmission(addr_20);
  Wire.write(val);
  Wire.endTransmission();
}

void Scanner() {

  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);  // PCF8574 7 bit address
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC);  // numbers of devices
  Serial.println(" device(s).");
}