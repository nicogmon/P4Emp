#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include "FastLED.h"
#include <Arduino_FreeRTOS.h>


#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];


#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

#define TURN_RIGHT 1
#define STRAIGHT 2
#define TURN_LEFT 3

#define REACTION_FREC 1
#define ULTRASONIC_FREC 20

// Enable/Disable motor control.
//  HIGH: motor control enabled
//  LOW: motor control disabled
#define PIN_Motor_STBY 3

// Group A Motors (Right Side)
// PIN_Motor_AIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_AIN_1 7
// PIN_Motor_PWMA: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMA 5

// Group B Motors (Left Side)
// PIN_Motor_BIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_BIN_1 8
// PIN_Motor_PWMB: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMB 6

#define TRIG_PIN 13  
#define ECHO_PIN 12 


long time = 0;
long dist = 0;

int last_dir = 0;

int data[3];

static void ReactionTask(void* pvParameters){
  TickType_t xLastWakeYime;

  while(1)
  {
    xLastWakeYime = xTaskGetTickCount();
    

    data[0] = analogRead(PIN_ITR20001_LEFT); 
    data[1] = analogRead(PIN_ITR20001_MIDDLE); 
    data[2] = analogRead(PIN_ITR20001_RIGHT);

    for (int i = 0 ; i < 3; i++){
      if (data[i] > 600){
        data[i] = 1;
      }
      else if (data[i] < 200){
        data[i] = 0;
      }else {
        //tenemos la opciond e considerar que ha visto algo de negro y por si acaso girar
        //pero esto igual es demasiado redundante o conflictivo con el comparar tambien 
        //los posibles datos con mas de un uno tipo 110 o 011 
        //de hecho habria que cabiar y ver que poner en casod e que sea 2 
        //data[i = 2]
      }
    }
    reaction(data[0], data[1], data[2]);

    Serial.println("reactionTask");
    xTaskDelayUntil(&xLastWakeYime, REACTION_FREC);
    
  }
}


static void DistanceTask(void* pvParameters){
  TickType_t xLastWakeYime;

  while(1)
  {

    xLastWakeYime = xTaskGetTickCount();
    

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);  //Enviamos un pulso de 10us
    digitalWrite(TRIG_PIN, LOW);
    time = pulseIn(ECHO_PIN, HIGH);
    dist = time / 59;  // distancia en cm
    if (dist < 15){
      Mover_Stop();
      return ;
    }

      Serial.println("reactionTask");
      xTaskDelayUntil(&xLastWakeYime, ULTRASONIC_FREC);
      
  }
  
}


void setup() {
  // put your setup code here, to run once:
  

  Serial.begin(9600);
  pinMode(PIN_ITR20001_LEFT, INPUT);
  pinMode(PIN_ITR20001_MIDDLE, INPUT);
  pinMode(PIN_ITR20001_RIGHT, INPUT);


  pinMode(PIN_Motor_STBY, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);
  digitalWrite (PIN_Motor_AIN_1, HIGH);
  digitalWrite (PIN_Motor_BIN_1, HIGH);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);



  xTaskCreate(
    ReactionTask,
    "ReactionTask",
    100,
    NULL,
    3,
    NULL
  );

  xTaskCreate(
    DistanceTask,
    "DistanceTask",
    100,
    NULL,
    2,
    NULL
  );

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  while(1) {

    if (Serial.available()) {

      char c = Serial.read();
      sendBuff += c;
      
      if (c == '}')  {            
        Serial.print("Received data in serial port from ESP32: ");
        Serial.println(sendBuff);

        // Set Red Green to LED
        FastLED.showColor(Color(0, 255, 0));
        sendBuff = "";
        break;
      } 

    }
  }




}



uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}


int reaction(int dato1, int dato2, int dato3){

  if (dist < 15){
    Mover_Stop();
    Serial.pritnln("")
    return 0;
  }
  if (dato1 == 1 && dato2 == 0){
    Pivotar_Izquierda();
    last_dir = 0;
    FastLED.showColor(Color(0,255,0));
    return TURN_RIGHT;
  }
  else if (dato1 == 1 && dato2 == 1){
    Girar_Izquierda();
    last_dir = 0;
    FastLED.showColor(Color(0,255,0));
    return TURN_RIGHT;
  }
  else if (dato3 == 1 && dato2 ==0){
    Pivotar_Derecha();
    last_dir = 1;
    FastLED.showColor(Color(0,255,0));
    return TURN_LEFT;
  }
  else if (dato3 == 1 && dato2 ==1){
    Girar_Derecha();
    last_dir = 1;
    FastLED.showColor(Color(0,255,0));
    return TURN_LEFT;
  }
  else if (dato2 == 1){
    Mover_Adelante();
    FastLED.showColor(Color(0,255,0));
    return STRAIGHT;
  }
  else if (dato1 == 0 && dato2 == 0 && dato3 == 0) {

    FastLED.showColor(Color(255,0,0));
    if (last_dir == 0){
      Arco_Izquierda();
    } else if (last_dir == 1){
      Arco_Derecha();
    } 
  }
  return 0;
    
}
  
void loop() {
  

  // put your main code here, to run repeatedly:

}



void Mover_Adelante()
{
  
  analogWrite (PIN_Motor_PWMA, 130);
  
  analogWrite (PIN_Motor_PWMB, 130);
}

void Arco_Izquierda()
{
 
  
  analogWrite (PIN_Motor_PWMA, 100);
  
  analogWrite (PIN_Motor_PWMB, 0);
 
}
void Girar_Izquierda()
{
 
  
  analogWrite (PIN_Motor_PWMA, 100);
  
  analogWrite (PIN_Motor_PWMB, 0);
 
}

void Girar_Derecha()
{

  
  analogWrite (PIN_Motor_PWMA,0);
  
  analogWrite (PIN_Motor_PWMB, 100);
}

void Arco_Derecha()
{

  
  analogWrite (PIN_Motor_PWMA, 0);
  
  analogWrite (PIN_Motor_PWMB, 100);
}


void Pivotar_Izquierda(){
  analogWrite (PIN_Motor_PWMA, 100);
  digitalWrite (PIN_Motor_BIN_1, LOW);
  analogWrite (PIN_Motor_PWMB, 30);
  digitalWrite (PIN_Motor_BIN_1, HIGH);
}
void Pivotar_Derecha(){
  analogWrite (PIN_Motor_PWMB, 100);
  digitalWrite (PIN_Motor_AIN_1, LOW);
  analogWrite (PIN_Motor_PWMA, 30);
  digitalWrite (PIN_Motor_AIN_1, HIGH);
 

}

void Mover_Stop()
{

  
  analogWrite (PIN_Motor_PWMA, 0);
  
  analogWrite (PIN_Motor_PWMB, 0);
}
