#include <Arduino.h>

/*
TODO:
* Test each MOSFET with a small setup
* Test the random() function with a couple of reboots
* Make a nice transient random shuffle between different color (combinations)
* Calibrate min/max values for potmeters
*/

constexpr int Analog1{A0};
constexpr int Analog2{A1};
constexpr int Analog3{A2};
constexpr int RandomSeedPin{A3}; // Unconnected

constexpr int Rd{3}; //TODO: Check if connection are soldered correctly
constexpr int Gr{5};
constexpr int Bl{6};

//PWM pins: 3,5,6,9,10,11

void setup()
{
  pinMode(Rd, OUTPUT);
  pinMode(Gr, OUTPUT);
  pinMode(Bl, OUTPUT);
}

void loop()
{
  digitalWrite(Rd, LOW);
  delay(250);
  digitalWrite(Rd, HIGH);
  delay(250);
}