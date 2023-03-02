#include <PIDController.h>
#include <PID_v1.h>
#include <NewPing.h>

const byte IRFL = 19;     // the IR Front sensor in left postion
const byte IRFR = 20;     // the IR Front sensor in right postion

const byte IRSL = 21;     // the IR Side sensor in left postion
const byte IRSR = 22;     // the IR Side sensor in right postion

// Encoder pins 2, 3 are hardware interrupts
const byte EncoderAL = 4;
const byte EncoderBL = 2;
const byte EncoderAR = 5;
const byte EncoderBR = 3;


const byte IN1 = 6; // Left Motor Postive 
const byte IN2 = 7; // Left Motor Negative 

const byte IN3 = 8; // Right Motor Postive 
const byte IN4 = 9; // Right Motor Negative

const byte ENL = 10;  // Enable for Left motor
const byte ENR = 11;  // Enable for Right motor


/***********FOR ULTRA SONIC*********/
const int leftEchoPin = 50;
const int leftTrigPin = 51;

const int rightEchoPin = 52;
const int rightTrigPin = 53;  // was A0 pin 

NewPing sonarR(rightTrigPin, rightEchoPin, 300);
NewPing sonarL(leftTrigPin, leftEchoPin, 300);

/*************FOR PIDs*************/
// Pid controller object
volatile long int encoder_countR = 0;
volatile long int encoder_countL = 0;

double rpmL = 0.0;
double rpmR = 0.0;

// Float for the desired RPM
double rpm_setL = 0;
double rpm_setR = 0;

// Byte for PWM of each Motor
double pwmL = 0;
double pwmR = 0;

// PID controller for each Motor
double kp = 2, ki = 5, kd = 1;

double error = 0, desired = 0, dPWM = 0;
volatile double currentError = 0;

PID pidL(&rpmL, &pwmL, &rpm_setL, kp, ki, kd, DIRECT);
PID pidR(&rpmR, &pwmR, &rpm_setR, kp, ki, kd, DIRECT);
PID pidNew(&error, &dPWM, &desired, kp, ki, kd, DIRECT);
