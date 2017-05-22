#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

// Servo pin mappings
#define SERVO_PIN   3 // Any pin on the Arduino or Gertboard will work.
#define MOTOR_PIN   3

// Receiver pin mappings
#define NRF_CE_PIN  4 // yellow
#define NRF_CSN_PIN 5 // orange

// Motor shield pin mappings
#define MOTOR_M1INA_PIN 7
#define MOTOR_M1INB_PIN 8
#define MOTOR_M1EN_PIN  6
#define MOTOR_M1CS_PIN  A0
#define MOTOR_M2INA_PIN 2
#define MOTOR_M2INB_PIN 0
#define MOTOR_M2EN_PIN  1
#define MOTOR_M2CS_PIN  A1

// Serial vals
#define BAUD_RATE 9600

// Receiver vals
#define NRF_PIPE 0xE8E8F0F0E1LL

// Joystick vals
#define JOY_MAX 1024

// Motor shield vals
#define MOTOR_MAX 400

// Servo vals
#define SERVO_PULSE_PERIOD 20000 // Microseconds
#define SERVO_PULSE_LEN 1000 // Microseconds
#define SERVO_PULSE_MIN 750 // Microseconds
#define SERVO_PULSE_HALF 1500 // Microseconds
#define SERVO_PULSE_MAX 2250 // Microseconds
#define SERVO_PULSE_RANGE SERVO_PULSE_MAX-SERVO_PULSE_MIN // Microseconds

// Global variables
int msg[3]; // [X, Y, BTN]
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
Servo motor;

void setup() {
  // Servo setup
//  pinMode(SERVO_PIN, OUTPUT);

  // Receiver setup
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1,NRF_PIPE);
  radio.startListening();

  // Motor setup
  motor.attach(MOTOR_PIN);
  // Setup logging
  Serial.begin(BAUD_RATE);
}

//int servoPulse(float frac){
//  if(frac < .33)
//    return SERVO_PULSE_MIN;
//  else if(frac < .66)
//    return SERVO_PULSE_HALF;
//  return SERVO_PULSE_MAX;
//}

//void updateServo(int joyPos){
//  int val = servoPulse(float(joyPos) / float(JOY_MAX));
//  
//  // Servos work by sending a 20 ms pulse.  
//  // 0.6 ms at the start of the pulse will turn the servo to the 0 degree position
//  // 0.89 ms at the start of the pulse will turn the servo to the 90 degree position 
//  // 2.39 ms at the start of the pulse will turn the servo to the 180 degree position 
//  // Turn voltage high to start the period and pulse
//  digitalWrite(SERVO_PIN, HIGH);
//
//  // Delay for the length of the pulse
//  delayMicroseconds(val);
//
//  // Turn the voltage low for the remainder of the pulse
//  digitalWrite(SERVO_PIN, LOW);
//
//  // Delay this for the remainder of the period so we don't
//  // send the next signal too soon or too late
//  delayMicroseconds(SERVO_PULSE_PERIOD - val);
//}

//void updateMotor(int joyPos){
//  int val = servoPulse(float(joyPos) / float(JOY_MAX));
//}

void dumpLog(){
  // Joystick info
  Serial.print(" X: ");
  Serial.print(msg[0]);
  Serial.print(" Y: ");
  Serial.print(msg[1]);
  Serial.print(" button: ");
  Serial.print(msg[2]);

}

void updateMotor(int joyPos){
  int pos = 0; 
  Serial.print("Forwarding");
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    motor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  Serial.print("Reversing");
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    motor.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
void loop() {
  if (radio.available()){
    
    radio.read(msg, 6);
//    updateServo(msg[0]);
    updateMotor(msg[1]);
    dumpLog();
    delay(100 - SERVO_PULSE_PERIOD/1000);
  }
}
