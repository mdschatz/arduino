#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

// Servo pin mappings
#define SERVO_PIN   6 // Any pin on the Arduino or Gertboard will work.
#define MOTOR_PIN   3

// Receiver pin mappings
#define NRF_CE_PIN  4 // yellow
#define NRF_CSN_PIN 5 // orange

// Serial vals
#define BAUD_RATE 9600

// Receiver vals
#define NRF_PIPE 0xE8E8F0F0E1LL

// Joystick vals
#define JOY_MOTOR_MIN -512
#define JOY_MOTOR_MAX 512
#define JOY_MIN 0
#define JOY_MAX 1024

// Servo vals
#define SERVO_PULSE_PERIOD 20000 // Microseconds
#define SERVO_PULSE_LEN 1000 // Microseconds
#define SERVO_PULSE_MIN 750 // Microseconds
#define SERVO_PULSE_HALF 1500 // Microseconds
#define SERVO_PULSE_MAX 2250 // Microseconds
#define SERVO_PULSE_RANGE SERVO_PULSE_MAX-SERVO_PULSE_MIN // Microseconds

#define MIN_SERVO 0
#define MAX_SERVO 180
#define MIN_MOTOR 700
#define MAX_MOTOR 2000

// Global variables
int msg[3]; // [X, Y, BTN]
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
Servo esc;
Servo servo;

int esc_set = false;

void setup() {
  // Servo setup
//  pinMode(SERVO_PIN, OUTPUT);

  // Receiver setup
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1,NRF_PIPE);
  radio.startListening();

  // Motor setup
  servo.attach(SERVO_PIN);
  esc.attach(MOTOR_PIN);
  // Setup logging
  Serial.begin(BAUD_RATE);
}

void dumpLog(){
  // Joystick info
  Serial.print(" X: ");
  Serial.print(msg[0]);
  Serial.print(" Y: ");
  Serial.print(msg[1]);
  Serial.print(" button: ");
  Serial.println(msg[2]);

}

void updateMotor(int joyPos){
  int throttle = map(max(joyPos-512, 0), 0, JOY_MOTOR_MAX, MIN_MOTOR, MAX_MOTOR);
  Serial.print("joy pos: ");
  Serial.print(joyPos);
  Serial.print(" ESC throttle: ");
  Serial.println(throttle);
  esc.write(throttle);
}

void updateServo(int joyPos){
  int pos = map(joyPos, JOY_MIN, JOY_MAX, MIN_SERVO, MAX_SERVO);
//  Serial.print("joyPos   : ");
//  Serial.print(joyPos);
//  Serial.print(" Servo pos: ");
//  Serial.println(max(MAX_SERVO-pos, 0));
  servo.write(max(MAX_SERVO-pos, 0));
  delay(15);
}
void loop() {
//  Serial.print("notavalable");
  if (radio.available()){
//    Serial.print("available");
    radio.read(msg, 6);
    updateServo(msg[0]);
    updateMotor(msg[1]);
    if (!esc_set) {
      esc_set = (msg[2] == 0);
    }
//    dumpLog();
//    delay(100 - SERVO_PULSE_PERIOD/1000);
  }
}
