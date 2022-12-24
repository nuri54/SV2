#include <Servo.h>
#include <Arduino.h>
#include <IRremote.h>
#include <Ultrasonic.h>

// TODO: Modus für Selbstfahren wenn Taste geklickt wird

////////// IR REMOTE CODES ELEGOO //////////
#define F               16736925      	// FORWARD
#define B               16754775      	// BACKWARD
#define L               16720605      	// LEFT
#define R               16761405      	// RIGHT
#define OK              16712445	      // STOP
#define PLUS            16732845        // INCREASE SPEED
#define MINUS           16728765        // DECREASE SPEED
#define AUTONOMOUS_ON   16750695        // TURN AUTONOM MODE ON  -  2
#define AUTONOMOUS_OFF  16730805        // TURN AUTONOM MODE OFF  -  0

/////////// IR OWN REMOTE CODES ///////////
#define F_OWN           2152644671      // FORWARD
#define B_OWN           2152626821      // BACKWARD
#define L_OWN           2152612031      // LEFT
#define R_OWN           2152596221      // RIGHT
#define OK_OWN          2152659461      // STOP

//////// IR SENSOR ////////
#define IR_SENSOR  9
IRrecv infrared(IR_SENSOR);
decode_results results;

/////// SERVO ///////
#define SERVO_PIN 10
Servo servo;
int rightDistance = 0, leftDistance = 0, middleDistance = 0;

////////////// ULTRASONIC //////////////
#define US_TRIGGER  13
#define US_ECHO     12
Ultrasonic Uschall(US_TRIGGER, US_ECHO);


#define US_TRIGGER_BACK  2
#define US_ECHO_BACK     4
Ultrasonic Uschall_Back(US_TRIGGER_BACK, US_ECHO_BACK);

//////// WHEELS AND MOTOR ////////
#define MOTOR_STBY_PIN          3
#define MOTOR_WHEELS_LEFT       5
#define MOTOR_WHEELS_RIGHT      6
// 1 = forward   0 = backward
#define LEFT_WHEELS_DIRECTION   8
#define RIGHT_WHEELS_DIRECTION  7

////////// SPEED //////////
unsigned char SPEED = 153;     // unsigned char 0 - 255
unsigned char TURN  = 50;

/////////// RGB ///////////
#define RED_PIN          A0
#define GREEN_PIN        A1
#define BLUE_PIN         A2
unsigned char red;
unsigned char green;
unsigned char blue;

bool DRIVING = LOW;
bool AUTONOM = LOW;

void sweep(){
  // 0° dann is er zu 90° nach rechts schauend -> nach vorne sind es 90°, nach links sind es 180°, nach rechts sind es 0°, hier aber 15° damit er nicht gegen das Bluetooth-Modul schlägt
  // Sweep to left
  servo.write(180);
  delay(500);
  leftDistance = Uschall.read(CM);
  Serial.print("Left: ");
  Serial.println(leftDistance);
  delay(500);
  // Middle
  servo.write(95);
  delay(500);
  middleDistance = Uschall.read(CM);
  Serial.print("Middle: ");
  Serial.println(middleDistance);
  delay(500);
  // Sweep to right
  servo.write(15);
  delay(500);
  rightDistance = Uschall.read(CM);
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
  DRIVING = HIGH;
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_LEFT, SPEED);
  digitalWrite(RIGHT_WHEELS_DIRECTION, HIGH);
  analogWrite(MOTOR_WHEELS_RIGHT, SPEED);
}

void backward(){
  digitalWrite(MOTOR_STBY_PIN, HIGH);
  digitalWrite(LEFT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_LEFT, SPEED);
  digitalWrite(RIGHT_WHEELS_DIRECTION, LOW);
  analogWrite(MOTOR_WHEELS_RIGHT, SPEED);
}

void stop(){
  DRIVING = LOW;
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
  if(SPEED > 255){
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
  if(SPEED < 0){
    SPEED = 51;
  }
}

void setup(){
  // TODO  nachgucken was das genau macht -> debug output at 9600 baud
  Serial.begin(9600);

  // servo.attach(SERVO_PIN, 700, 2400);   //500: 0 Grad, 2400: 180 Grad
  servo.attach(SERVO_PIN);
  servo.write(95);  // damit Sensor nach vorne gerichtet ist

  // start receiving infrared
  infrared.enableIRIn();

  pinMode(MOTOR_WHEELS_LEFT, OUTPUT);
  pinMode(MOTOR_WHEELS_RIGHT, OUTPUT);
  pinMode(LEFT_WHEELS_DIRECTION,OUTPUT);
  pinMode(RIGHT_WHEELS_DIRECTION, OUTPUT);
  pinMode(MOTOR_STBY_PIN, OUTPUT);

  pinMode(US_TRIGGER,OUTPUT);
  pinMode(US_ECHO, INPUT);

  pinMode(US_TRIGGER_BACK, OUTPUT);
  pinMode(US_ECHO_BACK, INPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  //GELB
  digitalWrite(RED_PIN, 0);
  digitalWrite(GREEN_PIN, 0);
  digitalWrite(BLUE_PIN, 255);
  delay(3000);
}

char wert;

void loop(){
  // AUTONOM AUSSCHALTEN
  if(AUTONOM == HIGH && results.value == AUTONOMOUS_OFF){
    stop();
    AUTONOM = LOW;
    red = 255;
    green = 0;
    blue = 255;
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
    infrared.resume();
  }

  // AUTONOM ANSCHALTEN
  if(AUTONOM == LOW && results.value == AUTONOMOUS_ON){
    AUTONOM = HIGH;
    stop();
    red = 255;
    green = 255;
    blue = 0;
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
    infrared.resume();
  }

  // AUTONOM FAHREN STOP + REAKTIONEN + PRÜFUNGEN
  if(Uschall.read(CM) < 80 && DRIVING == HIGH && AUTONOM == HIGH){
    stop();
    infrared.resume();
    sweep();
    if(rightDistance > leftDistance && rightDistance > middleDistance){
      turnRight();
      delay(1000);
      forward();
    }
    else if(leftDistance > rightDistance && leftDistance > middleDistance){
      turnLeft();
      delay(1000);
      forward();
    }
    else if(middleDistance > rightDistance && middleDistance > leftDistance){
      backward();
      delay(1000);
      if(rightDistance > leftDistance){
        turnRight();
        delay(1000);
        forward();
      }
      if(leftDistance > rightDistance){
        turnLeft();
        delay(1000);
        forward();
      }
    }
    servo.write(95);
  }

  // NICHT AUTONOM FAHREN STOP
  if(Uschall.read(CM) < 80 && AUTONOM == LOW){
    if(DRIVING == HIGH){
      stop();
    }
    red = 0;
    green = 255;
    blue = 255;
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
    infrared.resume();
  }

  // if(Uschall.read(CM) > 80 && AUTONOM == LOW){
  //   red = 255;
  //   green = 0;
  //   blue = 255;
  //   analogWrite(RED_PIN, red);
  //   analogWrite(GREEN_PIN, green);
  //   analogWrite(BLUE_PIN, blue);
  //   infrared.resume();
  // }


  /*// Für Bluetooth - klappt aber irgendwie noch nicht
  if(Serial.available() > 0){
    Serial.println(Serial.read());
  }*/


  // NICHT AUTONOM FAHREN 
  if(infrared.decode(&results) && AUTONOM == LOW){
    if(Uschall.read(CM) > 80){
      Serial.println(results.value);
      red = 255;
      green = 0;
      blue = 255;
      analogWrite(RED_PIN, red);
      analogWrite(GREEN_PIN, green);
      analogWrite(BLUE_PIN, blue);
      switch(results.value){
        case F_OWN : 
        case F : forward(); break;
        case B_OWN :
        case B : backward(); break;
        case L_OWN :
        case L : turnLeft(); break;
        case R_OWN :
        case R : turnRight();break;
        case OK_OWN :
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        default : break;
      }
    }

    if(Uschall.read(CM) < 80){
      Serial.println(results.value);
      switch(results.value){
        case B_OWN :
        case B : backward(); break;
        case L_OWN :
        case L : turnLeft(); break;
        case R_OWN :
        case R : turnRight();break;
        case OK_OWN :
        case OK : stop(); break;
        case PLUS : increaseSpeed(); break;
        case MINUS : decreaseSpeed(); break;
        default : break;
      }
    }

    infrared.resume();

  }
  
}
