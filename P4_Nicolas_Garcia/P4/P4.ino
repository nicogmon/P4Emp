#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include "FastLED.h"


#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];


#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

#define TURN_RIGHT 1
#define STRAIGHT 2
#define TURN_LEFT 3



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


ThreadController controller = ThreadController();
Thread Thread1 = Thread();
Thread Thread2 = Thread();

long time = 0;
long dist = 0;

int last_dir = 0;

int data[3];



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
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);




  Thread1.enabled = true;

  Thread1.setInterval(100);
  Thread1.onRun(callback_ultrasonidos);
  controller.add(&Thread1);  
  
  Thread2.enabled = true;
  Thread2.setInterval(10);
  Thread2.onRun(callback_infrarojos);
  controller.add(&Thread2);

  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);




}



uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}


int reaction(int dato1, int dato2, int dato3){

  if (dist < 15){
    Mover_Stop();
    return 0;
  }
  if (dato1 == 1 && dato2 == 0){
    Girar_Izquierda();
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
    Girar_Derecha();
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
      Pivotar_Izquierda();
    } else if (last_dir == 1){
      Pivotar_Derecha();
    } 
  }
  return 0;
    
}
  
void loop() {
  controller.run();
  
  


  // put your main code here, to run repeatedly:

}

void callback_ultrasonidos(){
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);  //Enviamos un pulso de 10us
  digitalWrite(TRIG_PIN, LOW);
  time = pulseIn(ECHO_PIN, HIGH);
  dist = time / 59;  // distancia en cm
  if (dist < 15){
    Mover_Stop();
    return ;
  }
  
  
}
void callback_infrarojos(){
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
  Serial.print(data[0]);
  Serial.print(" ");
  Serial.print(data[1]);
  Serial.print(" ");
  Serial.print(data[2]);
  Serial.println();
  
  reaction(data[0], data[1], data[2]);
}

void Mover_Adelante()
{
  digitalWrite (PIN_Motor_AIN_1, HIGH);
  analogWrite (PIN_Motor_PWMA, 120);
  digitalWrite (PIN_Motor_BIN_1, HIGH);
  analogWrite (PIN_Motor_PWMB, 120);
}

void Pivotar_Izquierda()
{
 
  digitalWrite (PIN_Motor_AIN_1, HIGH);
  analogWrite (PIN_Motor_PWMA, 100);
  digitalWrite (PIN_Motor_BIN_1, HIGH);
  analogWrite (PIN_Motor_PWMB, 30);
 
}
void Girar_Izquierda()
{
 
  digitalWrite (PIN_Motor_AIN_1, HIGH);
  analogWrite (PIN_Motor_PWMA, 100);
  digitalWrite (PIN_Motor_BIN_1,HIGH);
  analogWrite (PIN_Motor_PWMB, 0);
 
}

void Girar_Derecha()
{

  digitalWrite (PIN_Motor_AIN_1,HIGH);
  analogWrite (PIN_Motor_PWMA,0);
  digitalWrite (PIN_Motor_BIN_1,HIGH);
  analogWrite (PIN_Motor_PWMB, 100);
}

void Pivotar_Derecha()
{

  digitalWrite (PIN_Motor_AIN_1,HIGH);
  analogWrite (PIN_Motor_PWMA, 30);
  digitalWrite (PIN_Motor_BIN_1,HIGH);
  analogWrite (PIN_Motor_PWMB, 100);
}

void Mover_Stop()
{

  digitalWrite (PIN_Motor_AIN_1, LOW);
  analogWrite (PIN_Motor_PWMA, 0);
  digitalWrite (PIN_Motor_BIN_1, LOW);
  analogWrite (PIN_Motor_PWMB, 0);
}

