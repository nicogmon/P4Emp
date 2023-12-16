#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone_de_Ana";
const char* password = "anitaTNT";
const char* mqtt_server = "193.147.53.2";
const char* topic = "/SETR/2023/11/";
const int mqtt_port = 21883;
const char* mqttUser = "nicogmon";
const char* mqttPassword = "urjc2023";

#define RXD2 33
#define TXD2 4

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
  Serial2.write('1');
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  initWiFi();
  MQTT_connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Publicar el mensaje en el topic especificado
   // Example: Send the JSON payload when a condition is met
  client.loop();
  //sendJsonPayload();
  JsonCreate();
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

void JsonCreate(){//(int action, int value){
  String value;
  int action = 0;
  /*if (Serial2.available() > 0) {
    int action = Serial2.read();
    Serial.println(action);*/

  if (Serial2.available()) {
    String receivedString = Serial2.readStringUntil('\n');
    if (receivedString.length() > 0) {
      action = receivedString.toInt();
      Serial.print("Número recibido: ");
      Serial.println(action);

      if (action == 2 || action == 1 || action == 4 || action == 8){
        value = Serial2.readStringUntil('\n');
        value.trim();
        Serial.println(value);
        
      }
    }

  /*if (Serial2.available()) {
    value = Serial2.readStringUntil('\n');
    if (receivedString.length() > 0) {
      Serial.println(value);
    }
  }*/
    /*while (Serial2.available() > 0){ // Lee el primer byte (acción)
      value += Serial2.read(); // Lee el segundo byte (valor asociado a la acción)
    }*/
    

    StaticJsonDocument<200> jsonDoc;

    jsonDoc["team_name"] = "Dobbyziosos";
    jsonDoc["id"] = "11";
    jsonDoc["action"] = getAction(action);
    if (action == 1 || action == 4 ){
      jsonDoc["time"] = value;
    }
    if (action == 2){
      //char StrDistance[16] = "";
      //sprintf(StrDistance, "%u",value);
      jsonDoc["distance"] = value;
    }
    if (action == 8){
      //char StrValue[16] = "";
      //sprintf(StrValue, "%u",value);
      jsonDoc["value"] = value;
    }
    String jsonString = "";
    serializeJson(jsonDoc, jsonString);
    client.publish(topic, (char *)jsonString.c_str());

  }

}

String getAction(int action){
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

