//wifi INSERT YOUR SSID AND PASSWORD
#include <WiFi.h>
const char* ssid     = "YOUR SSID"; 
const char* password = "YOUR PASSWORD";
WiFiServer server(80);
int Initialised=false;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 1000;

#define PWM_CH1   2
#define PWM_CH2   3
#define PWM_CH3   4
#define PWM_CH4   7
#define PWM_Res   8
#define PWM_Freq  5000
#include <ESP32Servo.h>

//IR Sensors
const int IRHL = 35;
const int IRHR = 34;
const int IRVL = 12;
const int IRVR = 13;
const int IRB = 23; 
int IRHLvalue,IRHRvalue,IRVLvalue,IRVRvalue;
int IRBvalue;
int PrevTurn=1 ;

//Ultrasound sensors
const int TrigPinL = 25; 
const int EchoPinL = 26; 
const int TrigPinR = 33;
const int EchoPinR = 32;
int UltrasoundLvalue;
int UltrasoundRvalue;
float duration, distance;
long PreviousMillis = 0;
long MoveTime = 0;

//ESC pin
Servo ESC;
const int ESCpin =22;

//Motor pins
const int Motor1_IN1 = 2; // Right forward 
const int Motor1_IN2 = 4; // Right backward
const int Motor2_IN1 = 5; // Left forward 
const int Motor2_IN2 = 18; // Left backward

void IRdetect(){
  IRHLvalue = digitalRead(IRHL);
  IRHRvalue = digitalRead(IRHR);
  IRVLvalue = digitalRead(IRVL);
  IRVRvalue = digitalRead(IRVR);
  IRBvalue = digitalRead(IRB);
}

void UltrasoundL(){
  digitalWrite(TrigPinL, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPinL, LOW);
  duration = pulseIn(EchoPinL, HIGH);
  distance = (duration*.0343)/2;
  if (distance <15){  
    UltrasoundLvalue = 1;
        }
  else{
    UltrasoundLvalue = 0;
  }
}
void UltrasoundR(){
  digitalWrite(TrigPinR, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPinR, LOW);
  duration = pulseIn(EchoPinR, HIGH);
  distance = (duration*.0343)/2;
  if (distance <15){  
    UltrasoundRvalue = 1;
        }
  else{
    UltrasoundRvalue = 0;
  }
}

void EdgeCheck(){
    if ((IRVLvalue==1 || IRVRvalue==1)){ // roomba reaches the edge
      Brake();
      if (PrevTurn == 1){
        Backward();   // 90 degree right twice for U-turn
        delay(500);
        Right();
        delay(500);
        Backward();
        delay(500);
        Right();
        PrevTurn = 0;
    }
      else{
        Backward();
        delay(500);
        Left();
        delay(500);
        Backward();
        delay(500);
        Left();
        PrevTurn = 1;
      }
  }
}

void ObstacleCheck(){
  if (UltrasoundLvalue == 1 && UltrasoundRvalue == 1){
    Brake();
    delay(500);
    if (PrevTurn == 1) {
      Left();
      delay(1000);
      if (IRHRvalue==1){
        while (IRHRvalue==1){
          Forward();
          IRdetect();
        }
        Brake();
        delay(500);
      }
      
      PreviousMillis = millis();
      IRdetect();
      if (IRHRvalue==0){
        while(IRHRvalue==0){
          Forward();
          IRdetect();
        }
      }
      MoveTime = millis() - PreviousMillis;
      Brake();
      delay(500);
      Right();
      delay(1000);
      if (IRHRvalue==1){
        while (IRHRvalue==1){
          Forward();
          IRdetect();
          
        }
        Brake();
        delay(500);
      }
      IRdetect();
      if (IRHRvalue==0){
        while(IRHRvalue==0){
          Forward();
          IRdetect();
        }
      }
      Brake();
      Right();
      Forward();
      delay(MoveTime);
      Brake();
      Left();
      Forward();
    }
   if (PrevTurn == 0) {
      Right();
      delay(1000);
      if (IRHLvalue==1){
        while (IRHLvalue==1){
          Forward();
          IRdetect();
          
        }
        Brake();
        delay(500);
      }
      
      PreviousMillis = millis();
      IRdetect();
      if (IRHLvalue==0){
        while(IRHLvalue==0){
          Forward();
          IRdetect();
        }
      }
      MoveTime = millis() - PreviousMillis;
      Brake();
      delay(500);
      Left();
      delay(1000);
      if (IRHLvalue==1){
        while (IRHLvalue==1){
          Forward();
          IRdetect();
        }
        Brake();
        delay(500);
      }
      IRdetect();
      if (IRHLvalue==0){
        while(IRHLvalue==0){
          Forward();
          IRdetect();
        }
      }
      Brake();
      Left();
      Forward();
      delay(MoveTime);
      Brake();
      Right();
      Forward();
      }
    }
  }


void Brake(){
  ledcWrite(PWM_CH1, 0);
  ledcWrite(PWM_CH2, 0);
  ledcWrite(PWM_CH3, 0);
  ledcWrite(PWM_CH4, 0);
}

void Forward(){
  ledcWrite(PWM_CH1, 135);//right
  ledcWrite(PWM_CH4, 130);//left
}

void Backward(){
  ledcWrite(PWM_CH2,100);
  ledcWrite(PWM_CH3,100);
  for (int i=0; i<15;i++){
    IRdetect();
    if (IRBvalue==1){
      Brake();
      return;
  }
    delay(100);
  }
  Brake();
}

void Right() {
  ledcWrite(PWM_CH2, 100);
  ledcWrite(PWM_CH4, 100);
  delay(1300);
  Brake();
}
void Left(){
  ledcWrite(PWM_CH1, 100);
  ledcWrite(PWM_CH3, 100);
  delay(1300);
  Brake();
}

void setup() {
  // put your setup code here, to run once:

  WiFi.begin(ssid, password);
  server.begin();

  pinMode(Motor1_IN1, OUTPUT);
  pinMode(Motor1_IN2, OUTPUT);
  pinMode(Motor2_IN1, OUTPUT);
  pinMode(Motor2_IN2, OUTPUT);
  
  pinMode(IRHL, INPUT);
  pinMode(IRHR, INPUT);
  pinMode(IRVL, INPUT);
  pinMode(IRVR, INPUT);
  pinMode(IRB, INPUT);
  
  pinMode(TrigPinL, OUTPUT);
  pinMode(EchoPinL, INPUT);
  pinMode(TrigPinR, OUTPUT);
  pinMode(EchoPinR, INPUT);
  
  ledcAttachPin(Motor1_IN1, PWM_CH1);
  ledcSetup(PWM_CH1, PWM_Freq, PWM_Res );
  ledcAttachPin(Motor1_IN2, PWM_CH2);
  ledcSetup(PWM_CH2, PWM_Freq, PWM_Res );
  ledcAttachPin(Motor2_IN1, PWM_CH3);
  ledcSetup(PWM_CH3, PWM_Freq, PWM_Res );
  ledcAttachPin(Motor2_IN2, PWM_CH4);
  ledcSetup(PWM_CH4, PWM_Freq, PWM_Res );

  ESC.attach(22,1000,2000);
  ESC.write(0);
  delay(1000);
  
}
void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client= server.available();
  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    String Currentline ="";
    while (client.connected()&& currentTime - previousTime <= timeoutTime) { 
    //while (client.connected()) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') { 
          if (Currentline.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
             if (Initialised==false){
              //Start button, CAN BE EDITED
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: left;}");
              client.println(".button { background-color: #00FFFF; border: none; widtch:50%; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
              client.println("<p><a href=\"/START\"><button class=\"button\">START</button></a></p>");
            }
            if (Initialised==true){
              //Stop button, CAN BE EDITED 
              client.println("<style>html { font-family: Comic Sans; display: inline-block; margin: 0px auto; text-align: right;}");
              client.println(".button2 {background-color: #FF69B4; border: none; width:50%; padding: 16px 40px; ");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
              client.println("<p><a href=\"/STOP\"><button class=\"button2\">STOP</button></a></p>");
            }
            client.println();
            break;
          } else{
              Currentline="";
            } 
         }else if (c !='\r'){
            Currentline+=c;
          }
          
          if (Currentline.endsWith("GET /START")){
            Initialised=true;
            Forward();
            ESC.write(70);
            }
          if (Currentline.endsWith("GET /STOP")){
            Initialised=false;
            Brake();
            ESC.write(0);
          }
      }
    }
    client.stop();
  }
  
  if (Initialised==true){
    Forward();
    ESC.write(70);
    IRdetect();
    EdgeCheck();
    UltrasoundL();
    UltrasoundR();
    ObstacleCheck();
    Forward();
  }
  else{
    Brake();
    ESC.write(0);
  }
}
