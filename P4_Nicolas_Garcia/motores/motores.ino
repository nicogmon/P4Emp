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

void setup(){
  Serial.begin(9600);
  pinMode(PIN_Motor_STBY, OUTPUT);
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMA, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_PWMB, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);
}


void loop(){
  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_PWMA, HIGH);
  digitalWrite(PIN_Motor_BIN_1, HIGH);
  digitalWrite(PIN_Motor_PWMB, HIGH);
  delay(3000);
  digitalWrite(PIN_Motor_AIN_1, LOW);
  digitalWrite(PIN_Motor_PWMA, LOW);
  digitalWrite(PIN_Motor_BIN_1, LOW);
  digitalWrite(PIN_Motor_PWMB, LOW);
  delay(3000);
}