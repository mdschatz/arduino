#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "DualVNH5019MotorShield.h"

int servoPin = 3;
int recverPin1 = 0; //CE pin (yellow)
int recverPin2 = 5; //CSN pin (orange)

//Servo vars
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0;    // variable to store the servo position

//Receiver/Joystick vars
int msg[3];
RF24 radio(recverPin1,recverPin2);
const uint64_t pipe = 0xE8E8F0F0E1LL;

int maxX = 650;
int halfMaxX = maxX/2;
int maxY = 150;
int halfMaxY = maxY/2;

//Motor vars
//(M1INA, M1INB, M1EN, M1CS, M2INA, M2INB, M2EN, M2CS) 
DualVNH5019MotorShield md(2,4,6,A0,7,8,1,A1);
int motorSpeed = 0;

void setupServo() {
 myservo.attach(servoPin);
}

void setupReceiver(){
 Serial.begin(9600);
 radio.begin();
 radio.setPALevel(RF24_PA_LOW);
 radio.openReadingPipe(1,pipe);
 radio.startListening();
}

void setupMotor(){
  md.init();
}

void setup() {
  setupMotor();
  setupReceiver();
  setupServo();
}

void updateServo(int yPos){
  pos = (float(yPos) - float(halfMaxY)) / float(maxY) * float(maxY);
  Serial.print("servo: ");
  Serial.println(pos);
  myservo.write(pos);
  delay(15);
}

void stopIfFault()
{
  if(md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
}

void updateMotor(int xPos){
  motorSpeed = (float(xPos)-float(halfMaxX)) / float(maxY) * float(maxX);
  md.setM1Speed(motorSpeed);
  stopIfFault();
  Serial.print("M1 current: ");
  Serial.println(md.getM1CurrentMilliamps());
}

void loop() {
   if (radio.available()){
     radio.read(msg, 3);
     updateServo(msg[1]);
     updateMotor(msg[0]);
     Serial.print("X: ");
     Serial.print(msg[0]);
     Serial.print(" Y: ");
     Serial.println(msg[1]);
     delay(85);
   }
}
