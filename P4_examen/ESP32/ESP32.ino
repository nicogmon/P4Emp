#include "WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>

const char* ssid = "eduroam";
const char* mqtt_server = "193.147.53.2";
const char* topic = "/SETR/2023/11/";
const int mqtt_port = 21883;
const char* mqttUser = "";
const char* mqttPassword = "";

#define EAP_ANONYMOUS_IDENTITY "20220719anonymous@urjc.es" // leave as it is
#define EAP_IDENTITY ""    // Use your URJC email
#define EAP_PASSWORD ""            // User your URJC password
#define EAP_USERNAME ""    // Use your URJC email




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

ThreadController controller = ThreadController(false);
Thread Thread1 = Thread();

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int start_lap = 0;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
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

void callback_ping(){
  if (start_lap != 0){
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["team_name"] = "Dobbyziosos";
    jsonDoc["id"] = "11";
    jsonDoc["action"] = "PING";
    jsonDoc["time"] = millis();

    String jsonString = "";
    serializeJson(jsonDoc, jsonString);
    client.publish(topic, (char *)jsonString.c_str());
  }
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
  Serial2.println(1);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Thread1.enabled = true;
  Thread1.setInterval(4000);
  Thread1.onRun(callback_ping);
  controller.add(&Thread1);


  initWiFi();
  MQTT_connect();

}

void loop() {
  // put your main code here, to run repeatedly:

  controller.run();


  client.loop();

  JsonCreate();
}



void JsonCreate(){//(int action, int value){
  String value;
  int action = 0;
  double int_value = 0;
  float float_value = 0;


  if (Serial2.available()) {
    String receivedString = Serial2.readStringUntil('\n');
    if (receivedString.length() > 0) {
      action = receivedString.toInt();
      Serial.print("Número recibido: ");
      Serial.println(action);

      if (action == 2 || action == 1 || action == 4 ){
        value = Serial2.readStringUntil('\n');
        int_value = value.toInt();
        Serial.println(int_value);
        
      }
      if (action == 8){
        value = Serial2.readStringUntil('\n');
        float_value = round(value.toFloat() * 100) / 100;

      }
    }


    StaticJsonDocument<200> jsonDoc;

    jsonDoc["team_name"] = "Dobbyziosos";
    jsonDoc["id"] = "11";
    jsonDoc["action"] = getAction(action);
    if (action == 0){
      start_lap = 1;
    }
    if (action == 1 || action == 4 ){
      jsonDoc["time"] = int_value;
    }
    if (action == 2){
      jsonDoc["distance"] = int_value;
    }
    if (action == 8){
      jsonDoc["value"] = float_value;
    }
    String jsonString = "";
    serializeJson(jsonDoc, jsonString);
    client.publish(topic, (char *)jsonString.c_str());
     
    if (action == 1){
      Thread1.enabled = false;
    }

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

