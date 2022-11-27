#include <Arduino.h>

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


void setup() {
  pinMode(PIN_MOTOR_PWMA, OUTPUT);
  pinMode(PIN_MOTOR_PWMB, OUTPUT);
  pinMode(PIN_MOTOR_AIN_1,OUTPUT);
  pinMode(PIN_MOTOR_BIN_1, OUTPUT);
  pinMode(PIN_MOTOR_STBY, OUTPUT);

  digitalWrite(PIN_MOTOR_STBY, HIGH);

  digitalWrite(PIN_MOTOR_AIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMA, SPEED);

  
  digitalWrite(PIN_MOTOR_BIN_1, HIGH);
  analogWrite(PIN_MOTOR_PWMB, SPEED);


}


void loop() {
  // put your main code here, to run repeatedly:

}