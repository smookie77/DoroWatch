#include<WiFi.h>

int Number_of_users=0;

const char *AP_SSID="JXL_ESP32_WIFI";
const char *AP_PassWord="122232542";

//const char *ssid = "ESP32_AP_TEST";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.softAP(AP_SSID,AP_PassWord);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Number_of_users=WiFi.softAPgetStationNum();
  Serial.printf("%d\n",Number_of_users);
}
