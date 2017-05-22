#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include "DualVNH5019MotorShield.h"

//NOTE: Weird behaviour: 
//      if setupServo done last, you lose control of motor.  
//      if setupMotor last, you lose control of servo
//      This is because the two libraries clash over Timer1
//      Solution is to implement software PWM

//Servo pin mappings
#define SERVO_PIN   3 // Any pin on the Arduino or Gertboard will work.

//Receiver pin mappings
#define NRF_CE_PIN  4 //yellow
#define NRF_CSN_PIN 5 //orange

//Motor shield pin mappings
#define MOTOR_M1INA_PIN 7
#define MOTOR_M1INB_PIN 8
#define MOTOR_M1EN_PIN  6
#define MOTOR_M1CS_PIN  A0
#define MOTOR_M2INA_PIN 2
#define MOTOR_M2INB_PIN 0
#define MOTOR_M2EN_PIN  1
#define MOTOR_M2CS_PIN  A1

//Serial vals
#define BAUD_RATE 9600

//Receiver vals
#define NRF_PIPE 0xE8E8F0F0E1LL

//Joystick vals
#define JOY_MAX 1024

//Motor shield vals
#define MOTOR_MAX 400

//Servo vals
#define SERVO_PULSE_PERIOD 20000 //Microseconds
#define SERVO_PULSE_LEN 1000 //Microseconds
#define SERVO_PULSE_MIN 750 //Microseconds
#define SERVO_PULSE_HALF 1500 //Microseconds
#define SERVO_PULSE_MAX 2250 //Microseconds
#define SERVO_PULSE_RANGE SERVO_PULSE_MAX-SERVO_PULSE_MIN //Microseconds

//Global variables
int msg[3]; //[X, Y, BTN]
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
//DualVNH5019MotorShield md(MOTOR_M1INA_PIN, MOTOR_M1INB_PIN, MOTOR_M1EN_PIN, MOTOR_M1CS_PIN, 
//                          MOTOR_M2INA_PIN, MOTOR_M2INB_PIN, MOTOR_M2EN_PIN, MOTOR_M2CS_PIN);

void setup() {
  //Servo setup
  pinMode(SERVO_PIN, OUTPUT);

  //Motor shield setup
//  md.init();

  //Receiver setup
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1,NRF_PIPE);
  radio.startListening();

  //Setup logging
  Serial.begin(BAUD_RATE);
}

//int motorSpeed(float frac){
//  int check = (frac-0.5)*float(MOTOR_MAX)*2;
//  if(check > -25 && check < 25)
//    return 0;
//  return check;
//}

//void updateMotor(int joyPos){
//  int val = motorSpeed(float(joyPos) / float(JOY_MAX));
//  md.setM1Speed(val);
//
//  //Check if fault occurred and stop if so
////  if(md.getM1Fault())
////  {
////    Serial.println("M1 fault");
////    while(1);
////  }
//}

int servoPulse(float frac){
  if(frac < .33)
    return SERVO_PULSE_MIN;
  else if(frac < .66)
    return SERVO_PULSE_HALF;
  return SERVO_PULSE_MAX;
}

void updateServo(int joyPos){
  int val = servoPulse(float(joyPos) / float(JOY_MAX));
  
  // Servos work by sending a 20 ms pulse.  
  // 0.6 ms at the start of the pulse will turn the servo to the 0 degree position
  // 0.89 ms at the start of the pulse will turn the servo to the 90 degree position 
  // 2.39 ms at the start of the pulse will turn the servo to the 180 degree position 
  // Turn voltage high to start the period and pulse
  digitalWrite(SERVO_PIN, HIGH);

  // Delay for the length of the pulse
  delayMicroseconds(val);

  // Turn the voltage low for the remainder of the pulse
  digitalWrite(SERVO_PIN, LOW);

  // Delay this for the remainder of the period so we don't
  // send the next signal too soon or too late
  delayMicroseconds(SERVO_PULSE_PERIOD - val);
}

void dumpLog(){
  //Joystick info
  Serial.print(" X: ");
  Serial.print(msg[0]);
  Serial.print(" Y: ");
  Serial.print(msg[1]);
  Serial.print(" button: ");
  Serial.print(msg[2]);

  //Motor info
//  Serial.print(" M1Current: ");
//  Serial.println(md.getM1CurrentMilliamps());
  Serial.println();
}

void loop() {
  if (radio.available()){
    
    radio.read(msg, 6);
    updateServo(msg[0]);
//    updateMotor(msg[1]);
    dumpLog();
    delay(100 - SERVO_PULSE_PERIOD/1000);
  }
}
