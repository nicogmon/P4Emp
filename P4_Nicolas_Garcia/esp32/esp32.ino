#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "wifieif2";
const char* password = "Goox0sie_WZCGGh25680000";
const char* mqtt_server = "193.147.53.2";
const char* topic = "/SETR/2023/11/";
const int mqtt_port = 21883;
const char* mqttUser = "nicogmon";
const char* mqttPassword = "urjc2023";


#define START_LAP 0
#define END_LAP 1
#define OBSTACLE_DETECTED 2
#define LINE_LOST 3
#define PING 4
#define INIT_LINE_SEARCH 5
#define STOP_LINE_SEARCH 6
#define LINE_FOUND 7 
#define VISIBLE_LINE 8


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

void JsonCreate(int action, String value){
  StaticJsonDocument<200> jsonDoc;

  jsonDoc["team_name"] = "Dobbyziosos";
  jsonDoc["id"] = "11";
  jsonDoc["action"] = getAction(action);
  if (action == 1 || action == 4 ){
    //char  StrTime[16] = "";
    //sprintf(StrTime, "%u",value);
    jsonDoc["time"] = StrTime;
  }
  if (action == 2){
    //char StrDistance[16] = "";
    //sprintf(StrDistance, "%u",value);
    jsonDoc["distance"] = StrDistance;
  }
  if (action == 8){
    //char StrValue[16] = "";
    //sprintf(StrValue, "%u",value);
    jsonDoc["value"] = StrValue;
  }
  String jsonString = "";
  serializeJson(jsonDoc, jsonString);
  client.publish(topic, (char *)jsonString.c_str());

}

char * getAction(int action){
  if (action == 0){
    return "START_LAP";
  }
  if (action == 1){
    return "END_LAP";
  }
  if (action == 2){
    return "OBSTACLE_DETECTED";
  }
  if (action == 3){
    return "LINE_LOST";
  }
  if (action == 4){
    return "PING";
  }
  if (action == 5){
    return "INIT_LINE_SEARCH";
  }
  if (action == 6){
    return "STOP_LINE_SEARCH";
  }
  if (action == 7){
    return "LINE_FOUND";
  }
  if (action == 8){
    return "VISIBLE_LINE";
  }
  else{
    return "ERROR";
  }
  
}

