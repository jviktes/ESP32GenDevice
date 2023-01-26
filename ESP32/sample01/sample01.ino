/**
* ©2018 by Daniel Eichhorn - ThingPulse (https://thingpulse.com)
*/

#include <WiFi.h>
#include <HTTPClient.h>

#define IOT_HUB_NAME "vikhub"
#define DEVICE_NAME "esp32Temperature"
#define SAS_TOKEN "SharedAccessSignature sr=vikhub.azure-devices.net%2Fdevices%2Fesp32Temperature&sig=HRiMrJsiR7dSjuoAjycICpSyxmeyaN1O8mHkC0lfGAE%3D&se=1673328949"

const char* ssid = "dlink";
const char* password = ".MoNitor2?";

// Created with openssl s_client -showcerts -connect thingpulse.azure-devices.net:443
// and by picking the root certificate
const char* root_ca= \
"-----BEGIN CERTIFICATE-----\r\n"
"MIIFrTCCBJWgAwIBAgIQB9JqWPDx4GjFlGd8ZBuA+DANBgkqhkiG9w0BAQsFADBa\r\n"
"MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl\r\n"
"clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIy\r\n"
"MDQwNzAwMDAwMFoXDTI1MDUxMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxHjAcBgNV\r\n"
"BAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEbMBkGA1UEAxMSTVNGVCBCQUxUIFJT\r\n"
"MjU2IENBMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAwTgQW2vEtqjP\r\n"
"da6g6ZwoqAqb1mdoiFEqeYB8nex6Y0mSgW8NnF4C+MiF1MCFjSlWYgkIVycQ4E86\r\n"
"g7znUL1uVdkEol39U6UiogypLAsQh58fDe7goJrTE36BfQBeS9qx/rvfUPv/PhR7\r\n"
"4miZsc7nOUsaoMMS76LNymDhXD+imVseHynsmN2D2AJQZ/7nompXsn/NHIdQF2hq\r\n"
"FdLqb6tanGSZuCqCvnf9kJ7RNQipq8lozQhWSIQu6tQh2Rs+1iv2wEH7XJgSq8rc\r\n"
"snk4qI9uzfcvhPUNwU14a2rtnahcfUBHrjsaCsB7Ubgjqi+s9j3POkBCcBDW4x84\r\n"
"kAwhpGNYIp1abupXdBPPZYZ6VI3ViA9xeoql/ig8tlGLHsalfYb69HbmMGdrwDYm\r\n"
"f4YIuLmWSBBynmOJUcNSaDSEtKxERNwcUHzrrp9A9SaC4eg8ZK6J5R5mbVr5eegE\r\n"
"LzWTvPtXjiCXlfDvpr+PXLchwEkV3xjymdZd7eq+NmaSafY5mCm/C/KF5eQOhgaX\r\n"
"omERa2brYyUazJPQzoyHwFOdKpfNINqRg+TnzwXoapbZzVXdquafgUYuHOa28T8/\r\n"
"nv85tV20kxQMUy+ICV4anHsAibEpzgLuDV1Cl9CpoDMOL7fFYOpKXn/zLAG5ZyWW\r\n"
"6h426JHq5SKWV4z4utoSDiqMGsZpL1UCAwEAAaOCAX0wggF5MBIGA1UdEwEB/wQI\r\n"
"MAYBAf8CAQAwHQYDVR0OBBYEFEa78CwfKmRLIeiu3crbctSqIShcMB8GA1UdIwQY\r\n"
"MBaAFOWdWTCCR1jMrPoIVDaGezq1BE3wMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUE\r\n"
"FjAUBggrBgEFBQcDAQYIKwYBBQUHAwIweQYIKwYBBQUHAQEEbTBrMCQGCCsGAQUF\r\n"
"BzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQwYIKwYBBQUHMAKGN2h0dHA6\r\n"
"Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9CYWx0aW1vcmVDeWJlclRydXN0Um9vdC5j\r\n"
"cnQwOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t\r\n"
"bmlyb290MjAyNS5jcmwwPQYDVR0gBDYwNDALBglghkgBhv1sAgEwBwYFZ4EMAQEw\r\n"
"CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB\r\n"
"ADOP/3F1jZdadZfbmTfTRjJXHHjisxhiFlVL87cG9PLYIgn5E3xfuGKBnaGXnfdG\r\n"
"lPBQuwB2lKIUA1/JuE5CYF//6Kpa087EDV+Vn3pJ04VkIibNi48Efjs6ROSWPeSd\r\n"
"/CzqXB15LbeLB8v7tm4fsD7CRhERJJUfVkGP8s9249cy7V63SovqP6EYQhFxP0lw\r\n"
"JUbzhmMNx37mjnK9dMiKhNKhGQ2KUBdH/NuiuBL11h2mFowSiuNq6sGBNv9JwwKB\r\n"
"HQQ05jhzxXEJiw9lcCYg+2yIk5p6IY4ArdAwi4oZ4knEoyyUmOQy/MkTEdsSptaE\r\n"
"bOoBncTBFX2YkXulNYTPyz4=\r\n"
"-----END CERTIFICATE-----\r\n";

WiFiClientSecure client;

void sendRequest(String iothubName, String deviceName, String sasToken, String message) {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;
    String url = "https://" + iothubName + ".azure-devices.net/devices/" + deviceName + "/messages/events?api-version=2020-03-13";
    http.begin(url, root_ca); //Specify the URL and certificate
    http.addHeader("Authorization", sasToken);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(message);

    if (httpCode > 0) { //Check for the returning code

        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }

}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

}

void loop() {
  sendRequest(IOT_HUB_NAME, DEVICE_NAME, SAS_TOKEN, "{temperature: 24.5}");
  delay(10000);
}
