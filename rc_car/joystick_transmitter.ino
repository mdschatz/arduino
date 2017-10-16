#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

int msg[3];

//Receiver pin mappings
#define NRF_CE_PIN  4 //yellow
#define NRF_CSN_PIN 5 //orange
#define NRF_PIPE 0xE8E8F0F0E1LL

#define JOY_X_PIN   A0
#define JOY_Y_PIN   A1
#define JOY_BTN_PIN 2

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
void setup() {
  Serial.begin(9600);

  //Joystick
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);
  pinMode(JOY_BTN_PIN, INPUT_PULLUP);

  //Transmitter
  radio.begin();
  radio.openWritingPipe(NRF_PIPE);
}

void transmitInfo(int xPos, int yPos, int btnState){
  msg[0] = xPos;
  msg[1] = yPos;
  msg[2] = btnState;
  Serial.print("sending X: ");
  Serial.print(msg[0]);
  Serial.print(" Y: ");
  Serial.print(msg[1]);
  Serial.print(" but: ");
  Serial.println(msg[2]);
  radio.write(msg, 6);
}

void loop() {
  transmitInfo(analogRead(JOY_X_PIN), 
               analogRead(JOY_Y_PIN),
               digitalRead(JOY_BTN_PIN));

  delay(100); // add some delay between reads
}
