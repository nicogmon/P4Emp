#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "MiFibra-42C0-24G";
const char* password = "6QEK5kkw";
const char* mqtt_server = "193.147.53.2";
const char* topic = "/SETR/2023/11/";
const int mqtt_port = 21883;
const char* mqttUser = "nicogmon";
const char* mqttPassword = "urjc2023";


WiFiClient wifiClient;
//Adafruit_MQTT_Client mqttClient(&wifiClient, mqtt_server, mqtt_port);
PubSubClient client(wifiClient);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}



void MQTT_connect() {
  //int8_t ret;
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    client.connect("ESP32Client", mqttUser, mqttPassword );
    Serial.println("Intentando conexión MQTT...");
    delay(5000);
  }
  Serial.println("Conectado al servidor MQTT");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFi();
  MQTT_connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Publicar el mensaje en el topic especificado
   // Example: Send the JSON payload when a condition is met
  client.loop();
  sendJsonPayload();
}

void sendJsonPayload() {
  // Create a JSON document
  
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "Dobbyziosos";
  jsonDoc["id"] = "11";
  jsonDoc["action"] = "START_LAP";

  // Serialize the JSON document to a string
  String jsonString = "";
  serializeJson(jsonDoc, jsonString);

  // Publish the JSON payload to an MQTT topic
  client.publish(topic, (char *)jsonString.c_str());
  delay(5000);
}

void JsonCreate(char * action, int time, int distance, int value){
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["team_name"] = "Dobbyziosos";
  jsonDoc["id"] = "11";
  jsonDoc["action"] = action;
  if (time > 0 ){
    char  StrTime[16] = "";
    sprintf(StrTime, "%u",time);
    jsonDoc["time"] = StrTime;
  }
  if (distance > 0 ){
    char StrDistance[16] = "";
    sprintf(StrDistance, "%u",distance);
    jsonDoc["distance"] = StrDistance;
  }
  if (value > 0 ){
    char StrValue[16] = "";
    sprintf(StrValue, "%u",value);
    jsonDoc["value"] = StrValue;
  }
  String jsonString = "";
  serializeJson(jsonDoc, jsonString);
  client.publish(topic, (char *)jsonString.c_str());

}

