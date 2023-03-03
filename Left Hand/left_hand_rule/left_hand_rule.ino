const byte IRF1 = 19;
const byte IRF2 = 20;
const byte IRR = 21;
const byte IRF = 22;

const byte EncoderAL = 2;
const byte EncoderBL = 50;
const byte EncoderAR = 3;
const byte EncoderBR = 52;

const byte IN1 = 6;  // Motor Postive Left
const byte IN2 = 7;  // Motor Negative Left

const byte IN3 = 8;  // Motor Postive Right
const byte IN4 = 9;  // Motor Negative Right

const byte ENL = 10;  
const byte ENR = 11;

const byte triggerLeft = 12;
const byte echoLeft = 13;

const byte triggerRigt = 14;
const byte echoTight = 15;

int e=0, posR=0, posL=0;
int leftEncoderCounter = 0, rightEncoderCounter = 0;

int leftSpeed = 75, rightSpeed = 75;


void setup() {
  pinMode(IRF1, INPUT);
  pinMode(IRF2, INPUT);

  pinMode(EncoderAL, INPUT);
  pinMode(EncoderBL, INPUT);
  pinMode(EncoderAR, INPUT);
  pinMode(EncoderBR, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENL, OUTPUT);
  pinMode(ENR, OUTPUT);

  // Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(IRF1), wallFront, FALLING);
  attachInterrupt(digitalPinToInterrupt(IRF2), wallFront, FALLING);

  attachInterrupt(digitalPinToInterrupt(EncoderAL), updateEncR, RISING);
  attachInterrupt(digitalPinToInterrupt(EncoderBL), updateEncL, RISING);
  delay(2000);
}

void loop() {
  delay(100);
  if(wallLeft()){
    turnLeft();
    delay(50);
    moveForwad(leftSpeed - 10, rightSpeed - 10);
  }
  else if(wallRight()) {
    turnRight();
    delay(50);
    moveForwad(leftSpeed - 10, rightSpeed - 10);
  }
  else if(wallFront()){
    moveForward(leftSpeed, rightSpeed);
  }
  else{
    turnRight();
    turnRight();
  }
  
  
}

void speePid(){
  double leftWallDist  = getDistance(triggerLeft, echoLeft);
  double rightWallDist = getDistance(triggerRight, echoRight);

  int difference = rightWallDist - leftWallDist;

  if(difference >= 5){
    leftSpeed = 80, rightSpeed = 70;
  }

  if (difference < -5) { 
    leftSpeed = 70, rightSpeed = 80;
  }

  if (difference == 3 || difference == -4 ) {
    leftSpeed = 75, rightSpeed = 70;
  }

  if (difference == -3 || difference == -4 ) {
    leftSpeed = 70, rightSpeed = 75;
  }

  if (difference == 1 || difference == 2 || difference == 0) {
    leftSpeed = 75, rightSpeed = 75;
  }
}
void moveForwad(int speed1, int speed2){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    speePid();

    analogWrite(ENL, speed1);
    analogWrite(ENR, speed2);
}
// TODO: Check directions
void turnLeft(){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENL, leftSpeed);
    analogWrite(ENR, rightSpeed);

    delay(40);

    analogWrite(ENL, 0);
    analogWrite(ENR, 0);

}


void turnRight(){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENL, leftSpeed);
    analogWrite(ENR, rightSpeed);

    delay(40);

    analogWrite(ENL, 0);
    analogWrite(ENR, 0);
}

boolean wallFront() {
  return digitalRead(IRF1) == 1 && digitalRead(IRF2) == 1;
}

boolean wallLeft() {
  return digitalRead(IRL) == 1 ;
}

boolean wallRight() {
  return digitalRead(IRR) == 1 ;
}

void updateEncR(){
  rightEncoderCounter++;
}

void updateEncL(){
  leftEncoderCounter
}

double getDistance(byte trig, byte echo){
   digitalWrite(trig, HIGH);
   delay(1);
   digitalWrite(trig, LOW);
   TIME_TO_RESPOND= pulseIn(echo,HIGH);
   DIST = TIME_TO_RESPOND / 58.0;
   return DIST;
}