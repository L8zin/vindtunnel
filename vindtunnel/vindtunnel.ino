#include "HX711_MP.h"
#include "Servo.h"

#define MOTOR_PIN 10 // Pin for ECU control signal
#define DATA_PIN 4 // Pin lableled DT on HX711
#define CLOCK_PIN 7 // Pin lableled SCK on HX711

unsigned long previousTime = millis();
uint16_t timeInterval = 500; // time in milliseconds between measurements


HX711_MP scale(4); // Set # of calibration points

volatile float f;

Servo motor;


const byte numChars = 32;
char receivedChars[numChars];  // an array to store the received data

boolean newData = false;

int dataNumber = 0;  // new for this version


void setup() {

  motor.attach(MOTOR_PIN, 1000, 2000);
  motor.writeMicroseconds(0);

  Serial.begin(9600);
  Serial.println();


  scale.begin(DATA_PIN, CLOCK_PIN);

  //  Calibration
  //  adjust the data to your measurements
  //  setCalibrate(index, rawRead, weight);
  scale.setCalibrate(0, 45955, 0);
  scale.setCalibrate(1, 342957, 14);
  scale.setCalibrate(2, 447330, 23);
  scale.setCalibrate(3, 1005446, 67);

  Serial.println("Calibration values:");
  for (uint32_t raw = 30000; raw <= 1500000; raw += 10000) {
    Serial.print(raw);
    Serial.print("\t");
    Serial.println(scale.testCalibration(raw));
  }
  Serial.println("Calibration complete.");
  Serial.println("Plug in ECU and press enter.");
  while (!Serial.available() || !motor.attached())
    ;
  Serial.read();
  Serial.println("Starting...");
  delay(1000);
  Serial.println("Ready to go.");
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  motor.writeMicroseconds(dataNumber);
  recvWithEndMarker();
  showNewNumber();
  measure();
  
}

void measure() {
  unsigned long currentTime = millis();
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
