void GenerateJsonData(StaticJsonDocument<messageSize> data, char* outResJsonData) {

  char outJSONDataInternal[messageSize];

  Serial.println("GenerateJsonData...");
  StaticJsonDocument<messageSize> docRoot;
  //StaticJsonDocument<messageSize> data;
  docRoot["sensor_guid"] = uniqueGuid;

  docRoot["data"] = data;
  serializeJson(docRoot, outJSONDataInternal);

  //memcpy(outResJsonData, outJSONDataInternal, sizeof(outResJsonData));
  strncpy(outResJsonData, outJSONDataInternal, sizeof(outJSONDataInternal));

  Serial.println("serializeJson:");
  Serial.println(outResJsonData);
  //return outJSONData;
}