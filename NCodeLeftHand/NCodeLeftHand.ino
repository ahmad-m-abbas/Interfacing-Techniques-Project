#include <PIDController.h>
#include <PID_v1.h>
#include <NewPing.h>
#include "PinsAndVariables.h"

void setup() {
  Serial.begin(9600);

  // Encoder Pins as inputs 
  pinMode(EncoderAL,INPUT);
  pinMode(EncoderBL,INPUT);
  pinMode(EncoderAR,INPUT);
  pinMode(EncoderBR,INPUT);

  // Inputs pins that are connected with the H-bridge
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  // Enable pins as outputs ==> ENL is the enable 1&2 of the H-bridge, while ENR is the enable 3&4 of the H-bridge 
  pinMode(ENL,OUTPUT);
  pinMode(ENR,OUTPUT);

  // IR sensors as inputs the IRs with F is the front ones, while the IRs with S is the side ones
  pinMode(IRFL, INPUT);
  pinMode(IRFR, INPUT);
  pinMode(IRSL, INPUT);
  pinMode(IRSR, INPUT);

  // Ultra Sconic sensors 
  pinMode(leftTrigPin, OUTPUT);   // Sets the trigPin as an OUTPUT
  pinMode(leftEchoPin, INPUT);    // Sets the echoPin as an INPUT

  pinMode(rightTrigPin, OUTPUT);  // Sets the trigPin as an OUTPUT
  pinMode(rightEchoPin, INPUT);   // Sets the echoPin as an INPUT

  // set encoder pins B from both motor encoders as hardware interrupts. Put them as rising (we may change them to falling)
  attachInterrupt(digitalPinToInterrupt(EncoderBL), updateEncoderR, RISING);
  attachInterrupt(digitalPinToInterrupt(EncoderBR), updateEncoderL, RISING);

  // Set the H-bridge inputs to LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  
  // PIDs as automatic 
  pidL.SetMode(AUTOMATIC);
  pidR.SetMode(AUTOMATIC);
  pidNew.SetMode(AUTOMATIC);

  // Set the H-bridge inputs 2 and 4 to High
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);

  // limits for the PIDs (I am not sure which values should we put, we may change them)
  pidNew.SetOutputLimits(0, 20);
  pidL.SetOutputLimits(0, 75);
  pidR.SetOutputLimits(0, 75);
}

void loop() {
  delay(2000);
  start();
}

void start() {
  int rightCount = 0;
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  while (1) {
    if (rightCount == 3) {
      digitalWrite(IN2, LOW);     // important note: we may flip the inputs here !!
      digitalWrite(IN1, HIGH);
      digitalWrite(IN4, HIGH);
      digitalWrite(IN3, LOW);
      analogWrite(ENL, 80);
      analogWrite(ENR,80);
      //for (int i = 0; i < 50; i++) {
      //  analogWrite(6, 100);
      //  delay(200);
      //  analogWrite(6, 0);
      //  delay(200);
      //}
      delay(3000);
      stopAll();
      delay(1000);
      exit(0);
    }
    if (digitalRead(IRSL) == 1) {
      rightCount = 0;
      turn_left(90, 80);
      forward2(85);
    } else if (digitalRead(IRFL) == 1 && digitalRead(IRFR) == 1) {
      rightCount = 0;
      forward2(85);
    } else if (digitalRead(IRSR) == 1) {
      rightCount++;
      turn_right(90, 85);
      forward2(85);
    } else {
      rightCount = 0;
      turn_left(90, 85);
      turn_left(90, 85);
    }
  }
}

void forward2(int mspeed) {
  encoder_countL = 0;  //  reset counter A to zero
  encoder_countR = 0;  //  reset counter B to zero
  
  // Set Motor A forward
  //Serial.println(wallFront());
  
//  int type;
//  if (digitalRead(rightIr) == 1 && digitalRead(leftIr) == 0) { // digitalRead(rightIr) == 1 && digitalRead(leftIr) == 0
//    type = 0;
//  }  else if (digitalRead(leftIr) == 1 && digitalRead(rightIr) == 0) { //digitalRead(leftIr) == 1 && digitalRead(rightIr) == 0
//    type = 1;
//  } else if (digitalRead(leftIr) == 1 && digitalRead(rightIr) == 1) { //digitalRead(leftIr) == 1 && digitalRead(rightIr) == 1
//    type = 2;
//  } else {
//    type = 3;
//  }
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  // Set Motor B forward
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  encoder_countR = encoder_countL = 0;
  // Go forward until step value is reached
  while (true) {
    myPID();      // it was myPID(type);
    //slow M2, accelerate M1 speed
    //Serial.println("Eror: " + String(error) + "  dPwm: " + String(dPWM));
    if (currentError > 0) {
      analogWrite(ENL, mspeed + (int) (20 / 2));
      analogWrite(ENR, mspeed - (int) (20 / 2));
    }
    //slow M1, accelerate M2 speed
    else if (currentError < 0) {
      analogWrite(ENL, mspeed - (int) (20 / 2));
      analogWrite(ENR, mspeed + (int) (20 / 2));
    }
    else {
      analogWrite(ENL, 100);       // the value was 82 ?!
      analogWrite(ENR, 100);       // the value was 75 ?!
    }
    delay(0);
    //}
  }
  stopAll();
  delay(1000);
  // Stop when done
  //delay(1000);
}

void myPID() {
  int rightWall = wallRight();
  int leftWall = wallLeft();
  error = rightWall - leftWall;
  currentError = error;
  error = abs(error);
  //pidNew.Compute();         // !! delete the comments or not ?
}

void myPID2() {
  error = encoder_countR - encoder_countL;
  pidNew.Compute();
}

void updateEncoderL() {
  //  if (digitalRead(encoderPin3) == HIGH)
  encoder_countR++;
  //  else
  //    encoder_countL--;
}


void updateEncoderR() {
  //  if (digitalRead(encoderPin1) == HIGH)
  //    encoder_countR--;
  //  else
  encoder_countL++;
}


void turn_right(int steps, int mspeed) {
  encoder_countL = 0;  //  reset counter A to zero
  encoder_countR = 0;  //  reset counter B to zero
  
  // Set Motor A forward
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  // Set Motor B forward
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  error = 0;
  // Go forward until step value is reached
  while (steps > (encoder_countL + encoder_countR) / 2) {
    myPID2();
    //slow M2, accelerate M1 speed
    if (error >= 0) {
      if (steps > encoder_countL) {
        analogWrite(ENL, mspeed + (dPWM / 2));
      }
      if (steps > encoder_countR) {
        analogWrite(ENR, mspeed - (dPWM / 2));
      }
    }
    //slow M1, accelerate M2 speed
    if (error < 0) {
      if (steps > encoder_countL) {
        analogWrite(ENL, mspeed - (dPWM / 2));
      }
      if (steps > encoder_countR) {
        analogWrite(ENR, mspeed + (dPWM / 2));
      }
    }
    delay(0);
  }
  // Stop when done
  stopAll();
  delay(1000);      // too much ??
}

void turn_left(int steps, int mspeed) {
  encoder_countL = 0;  //  reset counter A to zero
  encoder_countR = 0;  //  reset counter B to zero
  
  // Set Motor A forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN1, HIGH);
  // Set Motor B forward
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  error = 0;
  // Go forward until step value is reached
  while (steps > (encoder_countL + encoder_countR) / 2) {
    myPID2();
    //slow M2, accelerate M1 speed
    if (error >= 0) {
      if (steps > encoder_countL) {
        analogWrite(ENL, mspeed + (dPWM / 2));
      }
      if (steps > encoder_countR) {
        analogWrite(ENR, mspeed - (dPWM / 2));
      }
    }
    //slow M1, accelerate M2 speed
    else {
      if (steps > encoder_countL) {
        analogWrite(ENL, mspeed - (dPWM / 2));
      }
      if (steps > encoder_countR) {
        analogWrite(ENR, mspeed + (dPWM / 2));
      }
    }
    delay(0);
  }
  // Stop when done
  stopAll();
  delay(1000);      // too much ??
}

void stopAll() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENL, 0);
  analogWrite(ENR, 0);
}

void PIdMotors() {
  pidL.Compute();
  // Serial.println(pwmL);  // no need 
  analogWrite(ENL, pwmL);

  pidR.Compute();
  analogWrite(ENR, pwmR);
}
int wallLeft() {
  return sonarL.ping_cm();
}

int wallRight() {
  return sonarR.ping_cm();
}
