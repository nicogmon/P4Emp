#define RXD2 33
#define TXD2 4


void setup() {

  // Regular serial connection to show traces for debug porpuses
  Serial.begin(9600);

  // Serial port to communicate with Arduino UNO
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  delay(5000);
  
  Serial2.print("{ 'test': " + String(millis()) + " }");
  Serial.print("Messase sent! to Arduino");
     
  
}

String sendBuff;

void loop() {

  // We always check if there is data in the serial buffer (max: 64 bytes)

  if (Serial2.available()) {

    char c = Serial2.read();
    sendBuff += c;
    
    if (c == '}')  {            
      Serial.print("Received data in serial port from Arduino: ");
      Serial.println(sendBuff);

      sendBuff = "";
    } 


  }
}

