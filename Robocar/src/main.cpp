#include <Arduino.h>
#include <Ultrasonic.h>

    
    Ultrasonic Uschall(13, 12);
    #define TRIGGER 13
    #define ECHO 12

 // speed linke reifen
    #define PIN_MOTOR_PWMA 5
    // speed rechte reifen
    #define PIN_MOTOR_PWMB 6


    // BIN and AIN 1 -> forward
    // BIN and AIN 0 -> backward

    // richtung reifen  links
    #define PIN_MOTOR_BIN_1 7
    // richtung reifen rechts 
    #define PIN_MOTOR_AIN_1 8

    #define PIN_MOTOR_STBY 3

    #define SPEED 255
    #define TURN 5


void setup() {
  // TODO  nachgucken was das genau macht
  Serial.begin(9600);  

  pinMode(PIN_MOTOR_PWMA, OUTPUT);
  pinMode(PIN_MOTOR_PWMB, OUTPUT);
  pinMode(PIN_MOTOR_AIN_1,OUTPUT);
  pinMode(PIN_MOTOR_BIN_1, OUTPUT);
  pinMode(PIN_MOTOR_STBY, OUTPUT);

  pinMode(TRIGGER,OUTPUT);
  pinMode(ECHO, INPUT);
}


void turnRight(){
  digitalWrite(PIN_MOTOR_STBY, HIGH);
  digitalWrite(PIN_MOTOR_AIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMA, SPEED/5);
  digitalWrite(PIN_MOTOR_BIN_1, LOW);
  analogWrite(PIN_MOTOR_PWMB, SPEED/5);
}

void turnLeft(){
  digitalWrite(PIN_MOTOR_STBY, HIGH);
  digitalWrite(PIN_MOTOR_AIN_1, LOW);
  analogWrite(PIN_MOTOR_PWMA, SPEED/5);
  digitalWrite(PIN_MOTOR_BIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMB, SPEED/5);
}

void driveStraightForward(){
  digitalWrite(PIN_MOTOR_STBY, HIGH);
  digitalWrite(PIN_MOTOR_AIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMA, SPEED);
  digitalWrite(PIN_MOTOR_BIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMB, SPEED);
}

void driveStraightBackwards(){
  digitalWrite(PIN_MOTOR_STBY, HIGH);
  digitalWrite(PIN_MOTOR_AIN_1, LOW);
  analogWrite(PIN_MOTOR_PWMA, SPEED);
  digitalWrite(PIN_MOTOR_BIN_1, LOW);
  analogWrite(PIN_MOTOR_PWMB, SPEED);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);

  while(true){
    if(Uschall.read(CM) <= 50){
      driveStraightBackwards();
      delay(1000);
      turnLeft();
      delay(1000);
    }
    else{
      driveStraightForward();
    }

/*
    if(Uschall.read(CM)>50){
      driveStraightForward();
    }
    else {
      driveStraightBackwards();
      delay(1000);
      turnLeft();
      delay(1000);
    }  */
  }
  
  /*
  driveStraightForward();
  delay(1000);
  turnRight();
  delay(1000);
  driveStraightForward();
  delay(1000);
  driveStraightBackwards();
  delay(1000);
  turnLeft();
  delay(1000);
  driveStraightForward();
  digitalWrite(PIN_MOTOR_STBY,LOW);
  */
  
}