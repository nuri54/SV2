#include <Servo.h>
#include <Arduino.h>
#include <IRremote.h>
#include <Ultrasonic.h>

////////// IR REMOTE CODES ELEGOO //////////
#define F               16736925      	// FORWARD
#define B               16754775      	// BACKWARD
#define L               16720605      	// LEFT
#define R               16761405      	// RIGHT
#define OK              16712445	      // STOP
#define PLUS            16732845        // INCREASE SPEED
#define MINUS           16728765        // DECREASE SPEED
#define AUTONOMOUS_ON   16750695        // TURN AUTONOM MODE ON  -  2

//////// IR SENSOR ////////
#define IR_SENSOR  9
IRrecv infrared(IR_SENSOR);
decode_results results;

/////// SERVO ///////
#define SERVO_PIN 10
Servo servo;
short int rightDistance = 0, leftDistance = 0, middleDistance = 0;

/////////////////////// ULTRASONIC ///////////////////////
#define US_TRIGGER_FRONT      13
#define US_ECHO_FRONT         12
Ultrasonic Uschall_Front(US_TRIGGER_FRONT, US_ECHO_FRONT);

#define US_TRIGGER_BACK       2
#define US_ECHO_BACK          4
Ultrasonic Uschall_Back(US_TRIGGER_BACK, US_ECHO_BACK);

//////// WHEELS AND MOTOR ////////
#define MOTOR_STBY_PIN          3
#define MOTOR_WHEELS_LEFT       5
#define MOTOR_WHEELS_RIGHT      6
// 1 = forward   0 = backward
#define LEFT_WHEELS_DIRECTION   8
#define RIGHT_WHEELS_DIRECTION  7

////////// SPEED //////////
unsigned char SPEED = 102;     // unsigned char 0 - 255
unsigned char TURN  = 50;

/////////// RGB ///////////
#define RED_PIN          A0
#define GREEN_PIN        A1
#define BLUE_PIN         A2
unsigned char RED;
unsigned char GREEN;
unsigned char BLUE;

bool DRIVING_FORWARD = LOW;
bool DRIVING_BACK = LOW;
bool AUTONOM = LOW;

void sweep(){
  // 0° dann is er zu 90° nach rechts schauend -> nach vorne sind es 90°, nach links sind es 180°, nach rechts sind es 0°, hier aber 15° damit er nicht gegen das Bluetooth-Modul schlägt
  // Sweep to left
  servo.write(180);
  delay(500);
  leftDistance = Uschall_Front.read(CM);
  Serial.print("Left: ");
  Serial.println(leftDistance);
  delay(500);
  // Middle
  servo.write(95);
  delay(500);
  middleDistance = Uschall_Front.read(CM);
  Serial.print("Middle: ");
  Serial.println(middleDistance);
  delay(500);
  // Sweep to right
  servo.write(15);
  delay(500);
  rightDistance = Uschall_Front.read(CM);
  Serial.print("Right: ");
  Serial.println(rightDistance);
  delay(500);
  servo.write(95);
}

void turnRight(){
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_LEFT, TURN);
  digitalWrite(RIGHT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_RIGHT, TURN);
}

void turnLeft(){
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_LEFT, TURN);
  digitalWrite(RIGHT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_RIGHT, TURN);
}

void forward(){
  DRIVING_FORWARD = HIGH;
  DRIVING_BACK = LOW;
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_LEFT, SPEED);
  digitalWrite(RIGHT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_RIGHT, SPEED);
}

void backward(){
  DRIVING_FORWARD = LOW;
  DRIVING_BACK = HIGH;
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_LEFT, SPEED);
  digitalWrite(RIGHT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_RIGHT, SPEED);
}

void stop(){
  DRIVING_FORWARD = LOW;
  DRIVING_BACK = LOW;
  digitalWrite(LEFT_WHEELS_DIRECTION, 1);
  analogWrite(MOTOR_WHEELS_LEFT, 0);
  digitalWrite(RIGHT_WHEELS_DIRECTION, 1);
  analogWrite(MOTOR_WHEELS_RIGHT, 0);
}

void increaseSpeed(){
  if(SPEED < 255){
    SPEED = SPEED + 51;
  }
  if(SPEED == 255){
    SPEED = 255;
  }
}

void decreaseSpeed(){
  if(SPEED > 0){
    SPEED = SPEED - 51;
  }
  if(SPEED == 0){
    SPEED = 51;
  }
}

void setup(){
  Serial.begin(9600);

  servo.attach(SERVO_PIN);
  servo.write(95);  // damit Sensor nach vorne gerichtet ist

  // start receiving infrared
  infrared.enableIRIn();

  pinMode(MOTOR_WHEELS_LEFT, OUTPUT);
  pinMode(MOTOR_WHEELS_RIGHT, OUTPUT);
  pinMode(LEFT_WHEELS_DIRECTION,OUTPUT);
  pinMode(RIGHT_WHEELS_DIRECTION, OUTPUT);
  pinMode(MOTOR_STBY_PIN, OUTPUT);

  pinMode(US_TRIGGER_FRONT,OUTPUT);
  pinMode(US_ECHO_FRONT, INPUT);

  pinMode(US_TRIGGER_BACK, OUTPUT);
  pinMode(US_ECHO_BACK, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  //GELB
  // analogWrite(RED_PIN, 0);
  // analogWrite(GREEN_PIN, 0);
  // analogWrite(BLUE_PIN, 255);
}

void loop(){
  // Für Bluetooth - klappt aber irgendwie noch nicht
  if(Serial.available() > 0){
    Serial.println(Serial.read());
  }

  // NICHT AUTONOM FAHREN
  if(infrared.decode(&results) && AUTONOM == LOW){
    if(Uschall_Front.read(CM) > 80){
      Serial.println(results.value);
      RED = 255;
      GREEN = 0;
      BLUE = 255;
      analogWrite(RED_PIN, RED);
      analogWrite(GREEN_PIN, GREEN);
      analogWrite(BLUE_PIN, BLUE);
      switch(results.value){
        case F : forward(); break;
        case L : turnLeft(); break;
        case R : turnRight();break;
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        case AUTONOMOUS_ON :  AUTONOM = HIGH; 
                              RED = 255;
                              GREEN = 255;
                              BLUE = 0;
                              analogWrite(RED_PIN, RED);
                              analogWrite(GREEN_PIN, GREEN);
                              analogWrite(BLUE_PIN, BLUE);
                              forward();
                              break;
        default : break;
      }
    }

    if(Uschall_Back.read(CM) > 80){
      Serial.println(results.value);
      RED = 255;
      GREEN = 0;
      BLUE = 255;
      analogWrite(RED_PIN, RED);
      analogWrite(GREEN_PIN, GREEN);
      analogWrite(BLUE_PIN, BLUE);
      switch(results.value){
        case B : backward(); break;
        case L : turnLeft(); break;
        case R : turnRight();break;
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        case AUTONOMOUS_ON :  AUTONOM = HIGH; 
                              RED = 255;
                              GREEN = 255;
                              BLUE = 0;
                              analogWrite(RED_PIN, RED);
                              analogWrite(GREEN_PIN, GREEN);
                              analogWrite(BLUE_PIN, BLUE);
                              forward();
                              break;
        default : break;
      }
    }

    if(Uschall_Front.read(CM) < 80){
      RED = 255;
      GREEN = 0;
      BLUE = 255;
      analogWrite(RED_PIN, RED);
      analogWrite(GREEN_PIN, GREEN);
      analogWrite(BLUE_PIN, BLUE);
      switch(results.value){
        case B : backward(); break;
        case L : turnLeft(); break;
        case R : turnRight();break;
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        case AUTONOMOUS_ON :  AUTONOM = HIGH;
                              RED = 255;
                              GREEN = 255;
                              BLUE = 0;
                              analogWrite(RED_PIN, RED);
                              analogWrite(GREEN_PIN, GREEN);
                              analogWrite(BLUE_PIN, BLUE);
                              break;
        default : break;
      }
    }

    if(Uschall_Back.read(CM) < 80){
      RED = 255;
      GREEN = 0;
      BLUE = 255;
      analogWrite(RED_PIN, RED);
      analogWrite(GREEN_PIN, GREEN);
      analogWrite(BLUE_PIN, BLUE);
      switch(results.value){
        case F : forward(); break;
        case L : turnLeft(); break;
        case R : turnRight();break;
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        case AUTONOMOUS_ON :  AUTONOM = HIGH;
                              RED = 255;
                              GREEN = 255;
                              BLUE = 0;
                              analogWrite(RED_PIN, RED);
                              analogWrite(GREEN_PIN, GREEN);
                              analogWrite(BLUE_PIN, BLUE);
                              break;
        default : break;
      }
    }
    infrared.resume();
  }

  // NICHT AUTONOM FAHREN - STOP VOR WAND
  if(AUTONOM == LOW && (Uschall_Front.read(CM) < 80 && DRIVING_FORWARD == HIGH) || (Uschall_Back.read(CM) < 80 && DRIVING_BACK == HIGH)){
    stop();
    RED = 0;
    GREEN = 255;
    BLUE = 255;
    analogWrite(RED_PIN, RED);
    analogWrite(GREEN_PIN, GREEN);
    analogWrite(BLUE_PIN, BLUE);
    infrared.resume();
  }
    //RÜCKWÄRTS FAHREN 
    //Dauer sweep

  // AUTONOM FAHREN
  if(AUTONOM == HIGH){
    if(results.value == OK){
      stop();
      AUTONOM = LOW;
      RED = 255;
      GREEN = 0;
      BLUE = 255;
      analogWrite(RED_PIN, RED);
      analogWrite(GREEN_PIN, GREEN);
      analogWrite(BLUE_PIN, BLUE);
    }
    else if(Uschall_Front.read(CM) > 80){
      forward();
    }
    else if(Uschall_Front.read(CM) < 80){
      stop();
      sweep();
      if(rightDistance > leftDistance && rightDistance > middleDistance && rightDistance > 80){
        turnRight();
        delay(1000);
        forward();
      }
      else if(leftDistance > rightDistance && leftDistance > middleDistance && leftDistance > 80){
        turnLeft();
        delay(1000);
        forward();
      }
      else{
        while(Uschall_Back.read(CM) > 80){
          backward();
        }
      
        stop();
        sweep();
        if(rightDistance > leftDistance && rightDistance > 80){
          turnRight();
          delay(1000);
          forward();
        }
        if(leftDistance > rightDistance && leftDistance > 80){
          turnLeft();
          delay(1000);
          forward();
        }
      }
      servo.write(95);
      infrared.resume();
    }
  }
 
}
