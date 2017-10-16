
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

enum State {
  FULL,
  EMPTY
};

State state;

#define WATER_PIN A0
#define MIN_WATER 400
#define MAX_WATER 600

#define LOW_WATER 640
#define HIGH_WATER 650

// pump setup
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
#define PUMP_ON 255
#define PUMP_OFF 0

void updatePump(int reading) {
  Serial.print("READING: ");
  Serial.print(reading);
  

  // update state
  Serial.print(" STATE: ");
  switch(state) {
    case EMPTY: 
      Serial.print("EMPTY");
      state = reading < HIGH_WATER ? EMPTY : FULL; 
      break;
    case FULL: 
      Serial.print("FULL");
      state = reading < LOW_WATER ? EMPTY : FULL; 
      break;
  }

  // update pump
  Serial.print(" PUMP: ");
  switch(state) {
    case EMPTY: 
      Serial.print("ON");
      myMotor->setSpeed(PUMP_ON);
      myMotor->run(FORWARD);
      break;
    case FULL:
      Serial.print("OFF");
      myMotor->run(RELEASE);
      break;
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // set up water level
  state = EMPTY;
  
  // set up pump
  AFMS.begin();
}

void loop() {
  updatePump(analogRead(WATER_PIN));
}
