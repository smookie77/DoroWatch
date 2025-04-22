#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <secrets.h>

// NTP
const long utcOffsetInSeconds = 3600;
const char* ntpServer = "pool.ntp.org";
WiFiUDP udp;
NTPClient timeClient(udp, ntpServer, utcOffsetInSeconds);

// Wi-Fi client for MQTT connection
WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

// Wi-Fi connection setup
void setup_wifi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  IPAddress ntpIP, mqttBrokerIP;
  if (WiFi.hostByName(ntpServer, ntpIP)) {
    Serial.print("Resolved NTP IP: ");
    Serial.println(ntpIP);
  } else {
    Serial.println("DNS resolution failed for NTP server");
  }

  if (WiFi.hostByName(mqtt_broker, mqttBrokerIP)) {
    Serial.print("Resolved MQTT Broker IP: ");
    Serial.println(mqttBrokerIP);
  } else {
    Serial.println("DNS resolution failed for MQTT broker");
  }
}

// MQTT setup
void connectToMQTT() {
  String client_id = "esp32-client-" + String(WiFi.macAddress());
  Serial.printf("Connecting to MQTT Broker as %s...\n", client_id.c_str());

  if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT broker!");
    mqtt_client.subscribe(mqtt_topic);
    mqtt_client.publish(mqtt_topic, "Connected");
  } else {
    Serial.print("Failed to connect, MQTT error code: ");
    Serial.println(mqtt_client.state());
    delay(5000);
  }
}

// MQTT message callback
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println("\n-----------------------");
}

// Get status based on touch
int status(int check) {
  if (touchRead(3) > 100000) {
    ///Serial.println(">>> Touched!");
    check = 1;
  } else {
    check = 0;
  }
  return check;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  timeClient.begin();
  timeClient.update();


  espClient.setCACert(ca_cert);
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost, reconnecting...");
    WiFi.disconnect();
    setup_wifi();
  }

  if (!mqtt_client.connected()) {
    Serial.println("MQTT lost, reconnecting...");
    connectToMQTT();
  }

  mqtt_client.loop();

  static int send;
  int check = status(send);

  if (send != check) {
    mqtt_client.publish(mqtt_topic, String(check).c_str());
    Serial.println("Status changed");
    send = check;
  }


  delay(1000);  // Keep loop responsive
}

