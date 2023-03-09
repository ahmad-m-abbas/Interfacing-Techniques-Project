#include <PIDController.h>
#include <PID_v1.h>
#include <NewPing.h>

/*********** IR PINS *********/
const byte IRFL = 19;  // the IR Front sensor in left postion
const byte IRFR = 20;  // the IR Front sensor in right postion

const byte IRSL = 22;  // the IR Side sensor in left postion
const byte IRSR = 21;  // the IR Side sensor in right postion

/*********** ENCODER PINS *********/
const byte LEFT_ENCODER_A = 2;  // hardware interrupt
const byte LEFT_ENCODER_B = 50;
const byte RIGHT_ENCODER_A = 3;  // hardware interrupt
const byte RIGHT_ENCODER_B = 52;

/*********** MOTOR PINS *********/
const byte IN1 = 6;  // Left Motor Negative
const byte IN2 = 7;  // Left Motor Positive

const byte IN3 = 8;  // Right Motor Negative
const byte IN4 = 9;  // Right Motor Positive

const byte ENL = 10;  // Enable for Left motor
const byte ENR = 11;  // Enable for Right motor

/*********** ULTRASONIC  ***********/
const int LEFT_ECHO_PIN = 13;
const int LEFT_TRIG_PIN = 12;

const int RIGHT_ECHO_PIN = 15;
const int RIGHT_TRIG_PIN = 14;

NewPing sonarR(RIGHT_TRIG_PIN, RIGHT_ECHO_PIN, 300);
NewPing sonarL(LEFT_TRIG_PIN, LEFT_ECHO_PIN, 300);

/************* FOR PIDs *************/
// encoder counts
volatile long int encoderCountR = 0;
volatile long int encoderCountL = 0;

volatile long int prevEncoderCountR = -1000000;
volatile long int prevEncoderCountL = -1000000;

// PID controller for each Motor
double kp = 2, ki = 5, kd = 1;

double error = 0, desired = 0, dPWM = 0;
volatile double currentError = 0;

PID pidNew(&error, &dPWM, &desired, kp, ki, kd, DIRECT);

int timerCounter = 42098;

volatile int state = 0;  // 0 => forward, 1=> left, 2=> right

void setup() {
  Serial.begin(9600);

  cli();
  // Encoder Pins as inputs
  pinMode(LEFT_ENCODER_A, INPUT);
  pinMode(LEFT_ENCODER_B, INPUT);
  pinMode(RIGHT_ENCODER_A, INPUT);
  pinMode(RIGHT_ENCODER_B, INPUT);

  // H-bridge pins as output
  // Motors positive and negative pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  // Enable pins
  pinMode(ENL, OUTPUT);
  pinMode(ENR, OUTPUT);

  // IR sensors as inputs
  pinMode(IRFL, INPUT);
  pinMode(IRFR, INPUT);
  pinMode(IRSL, INPUT);
  pinMode(IRSR, INPUT);

  // Ultra Sconic sensors
  pinMode(LEFT_TRIG_PIN, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(LEFT_ECHO_PIN, INPUT);   // Sets the echoPin as an INPUT

  pinMode(RIGHT_TRIG_PIN, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(RIGHT_ECHO_PIN, INPUT);   // Sets the echoPin as an INPUT

  // Set the H-bridge inputs to LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // PIDs as automatic
  pidNew.SetMode(AUTOMATIC);

  // Set the H-bridge inputs 2 and 4 to High
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);

  // limits for the PIDs (I am not sure which values should we put, we may change them)
  pidNew.SetOutputLimits(0, 20);


  // set encoder pins B from both motor encoders as hardware interrupts. Put them as rising (we may change them to falling)
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_A), updateEncoderR, RISING);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_A), updateEncoderL, RISING);

  TCCR3A = 0;
  TCCR3B = 0;

  TCNT3 = timerCounter;                 // preload timer
  TCCR3B |= (1 << CS12) | (1 << CS10);  // 1024 prescaler
  TIMSK3 |= (1 << TOIE1);               // enable timer overflow interrupt

  sei();
  delay(1000);
}

int rightCount = 0;  // number of consecutive right turns
int leftCount = 0;   // number of consecutive left turns

void loop() {
  // forward2(80, 225);
  if (rightCount == 3 || leftCount == 3) {  // if goal is reached
    stopAll();
    delay(500);
    exit(0);
  }
  if (digitalRead(IRSL) == 1) {  // if the left is clear
    leftCount++;
    rightCount = 0;
    state = 1;
    turn_left(80, 85);
    forward2(75, 225);
  } else if (digitalRead(IRFL) == 1 && digitalRead(IRFR) == 1) {  // if front is clear
    rightCount = 0;
    leftCount = 0;
    state = 0;
    forward(80);
  } else if (digitalRead(IRSR) == 1) {  // if right is clear
    rightCount++;
    leftCount = 0;
    state = 2;
    turn_right(80, 85);
    forward2(75, 225);
    //stopAll();
  } else {
    leftCount = 0;
    rightCount = 0;
    state = -1;  // **********************************************************************
    turn_left(90, 85);
    turn_left(90, 85);
  }
}

// move forward until reaching a wall or reaching the maximum number of steps.
void forward2(int mspeed, int steps) {
  // Set Motor A forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Set Motor B forward/
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  encoderCountR = encoderCountL = 0;
  while (digitalRead(IRFL) && digitalRead(IRFR) && (encoderCountR + encoderCountL) / 2 <= steps) {
    myPID(mspeed);
  }
  stopAll();
  delay(500);
}

void forward(int mspeed) {
  encoderCountL = 0;  //  reset counter A to zero
  encoderCountR = 0;  //  reset counter B to zero

  // Set Motor A forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Set Motor B forward
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  
  while (digitalRead(IRFL) && digitalRead(IRFR) && digitalRead(IRSL) == 0) {  // (encoderCountR + encoderCountL)/2 <=200
    myPID(mspeed);
  }
  if (digitalRead(IRSL)) {
    forward2(mspeed, 150);
  }

  stopAll();
  delay(500);
}

void myPID(int mspeed) {
  if (digitalRead(IRSL) && digitalRead(IRSR)) {
    analogWrite(ENL, mspeed);
    analogWrite(ENR, mspeed);
    return;
  }
  int rightWall = wallRight();
  int leftWall = wallLeft();
  currentError = (rightWall % 16) - (leftWall % 16);
  if (rightWall > 100 && IRSR == 0) {
    currentError = -1;
  }
  if (leftWall > 100 && IRSL == 0) {
    currentError = 1;
  }
  if (rightWall > 16) {
    currentError = 5 - leftWall;
  }

  //slow M2, accelerate M1 speed
  if (currentError > 0) {
    analogWrite(ENL, mspeed + 10);
    analogWrite(ENR, mspeed - 10);
  }
  //slow M1, accelerate M2 speed
  else if (currentError < 0) {
    analogWrite(ENL, mspeed - 10);
    analogWrite(ENR, mspeed + 10);
  } else {
    analogWrite(ENL, 70);
    analogWrite(ENR, 70);
  }
}

void myPID2() {
  error = encoderCountR - encoderCountL;
  pidNew.Compute();
}

void updateEncoderL() {
  encoderCountR++;
}


void updateEncoderR() {
  encoderCountL++;
}


void turn_right(int steps, int mspeed) {
  encoderCountL = 0;  //  reset counter A to zero
  encoderCountR = 0;  //  reset counter B to zero

  // Set Motor A forward
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  // Set Motor B forward
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  error = 0;
  // Go forward until step value is reached
  while (steps > (encoderCountL + encoderCountR) / 2) {
    myPID2();

    //slow M2, accelerate M1 speed
    if (error >= 0) {
      if (steps > encoderCountL) {
        analogWrite(ENL, mspeed + (dPWM / 2));
      }
      if (steps > encoderCountR) {
        analogWrite(ENR, mspeed - (dPWM / 2));
      }
    }
    //slow M1, accelerate M2 speed
    if (error < 0) {
      if (steps > encoderCountL) {
        analogWrite(ENL, mspeed - (dPWM / 2));
      }
      if (steps > encoderCountR) {
        analogWrite(ENR, mspeed + (dPWM / 2));
      }
    }
    delay(0);
  }
  // Stop when done
  stopAll();
  delay(500);
}

void turn_left(int steps, int mspeed) {
  encoderCountL = 0;  //  reset counter A to zero
  encoderCountR = 0;  //  reset counter B to zero

  // Set Motor A forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, HIGH);
  // Set Motor B forward
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  error = 0;
  // Go forward until step value is reached
  while (steps > (encoderCountL + encoderCountR) / 2) {
    myPID2();
    //slow M2, accelerate M1 speed
    if (error >= 0) {
      if (steps > encoderCountL) {
        analogWrite(ENL, mspeed + (dPWM / 2));
      }
      if (steps > encoderCountR) {
        analogWrite(ENR, mspeed - (dPWM / 2));
      }
    }
    //slow M1, accelerate M2 speed
    else {
      if (steps > encoderCountL) {
        analogWrite(ENL, mspeed - (dPWM / 2));
      }
      if (steps > encoderCountR) {
        analogWrite(ENR, mspeed + (dPWM / 2));
      }
    }
  }
  // Stop when done
  stopAll();
  delay(500);
}

void stopAll() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENL, 0);
  analogWrite(ENR, 0);
}

int wallLeft() {
  return sonarL.ping_cm();
}

int wallRight() {
  return sonarR.ping_cm();
}

ISR(TIMER3_OVF_vect) {
  if (prevEncoderCountL == encoderCountL && prevEncoderCountR == encoderCountR) {
    bool leftIR = digitalRead(IRSL);
    bool rightIR = digitalRead(IRSR);
    int leftWall = wallLeft();
    int rightWall = wallRight();
    int in1 = !digitalRead(IN1);
    int in2 = !digitalRead(IN2);
    int in3 = !digitalRead(IN3);
    int in4 = !digitalRead(IN4);
    for (int i = 0; i < 25000; i++) {
      digitalWrite(IN1, in1);
      digitalWrite(IN2, in2);
      digitalWrite(IN3, in3);
      digitalWrite(IN4, in4);
      analogWrite(ENL, 75);
      analogWrite(ENR, 75);
    }
    stopAll();
    if (leftIR && leftWall < 5) {
      for (int i = 0; i < 20000; i++) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(ENL, 75);
        analogWrite(ENR, 75);
      }
    } else if (rightIR && rightWall < 5) {
      for (int i = 0; i < 20000; i++) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENL, 75);
        analogWrite(ENR, 75);
      }
    }
    if (state > 0) {
      for (int i = 0; i < 25000 && digitalRead(IRFL) && digitalRead(IRFR); i++) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENL, 75);
        analogWrite(ENR, 75);
      }
    }


    stopAll();
    analogWrite(ENL, 0);
    analogWrite(ENR, 0);
    digitalWrite(IN1, !in1);
    digitalWrite(IN2, !in2);
    digitalWrite(IN3, !in3);
    digitalWrite(IN4, !in4);
  }
  prevEncoderCountR = encoderCountR;
  prevEncoderCountL = encoderCountL;
  TCNT3 = timerCounter;
}