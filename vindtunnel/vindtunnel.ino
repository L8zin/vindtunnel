//
//    FILE: HX_MP_plotter.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: HX711 demo
//     URL: https://github.com/RobTillaart/HX711_MP


#include "HX711_MP.h"
#include "Servo.h"

#define MOTOR_PIN 10
#define DATA_PIN 4
#define CLOCK_PIN 7

unsigned long previousTime = millis();
uint16_t timeInterval = 500;


HX711_MP scale(4);


//  adjust pins to your setup.
uint8_t dataPin = 4;
uint8_t clockPin = 7;

volatile float f;

Servo motor;


const byte numChars = 32;
char receivedChars[numChars];  // an array to store the received data

boolean newData = false;

int dataNumber = 0;  // new for this version


void setup() {

  Serial.begin(115200);
  Serial.println()


  scale.begin(dataPin, clockPin);

  //  Calibration
  //  adjust the data to your measurements
  //  setCalibrate(index, rawRead, weight);
  scale.setCalibrate(0, 45955, 0);
  scale.setCalibrate(1, 342957, 14);
  scale.setCalibrate(2, 447330, 23);
  scale.setCalibrate(3, 1005446, 67);

  for (uint32_t raw = 0; raw <= 00; raw += 1000) {
    Serial.print(raw);
    Serial.print("\t");
    Serial.println(scale.testCalibration(raw));
  }
  delay(5000);

  Serial.println("init");
  motor.attach(MOTOR_PIN);
  delay(500);
  Serial.println("plug in motor and press enter");
  while (!Serial.available())
    ;
  Serial.read();
  Serial.println("starting");
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  motor.writeMicroseconds(dataNumber);
  recvWithEndMarker();
  showNewNumber();
  if (currentTime - previousTime > timeInterval) {
    previousTime = currentTime;
    f = scale.get_units(5);
    Serial.println(f);
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  if (Serial.available() > 0) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewNumber() {
  if (newData == true) {
    dataNumber = 0;                    // new for this version
    dataNumber = atoi(receivedChars);  // new for this version
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    Serial.print("Data as Number ... ");  // new for this version
    Serial.println(dataNumber);           // new for this version
    newData = false;
  }
}
