#include "arduino_secrets.h"
/*
  The following variables are automatically generated and updated when changes are made to the Thing

  CloudVolume consumption;
  CloudVolume consumptionTotal;
  CloudLength distance;
  CloudCounter drinkCount;
  CloudCounter fillCount;

*/



#include "thingProperties.h"

// Include NewPing Library for distance sesor
#include "NewPing.h"

// Include the Servo library to control the water pump
#include <Servo.h>

// Define pins for distance sensor to detect water level
#define TRIGGER_PIN 9
#define ECHO_PIN 8

// Maximum distance to water to ping for (in centimeters).
#define MAX_DISTANCE 40

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
// Create a servo object
Servo Servo1;

int motionPin = 7;    // the pin the motion sensor is attached to
int servoPin = 3;     // the pin the servo motor is attached to

//declare distance variables
float duration, distanceNew, distanceCheck;
float area = 320;     //width and length of the dog bowl

//declare motion variables
int motion = 0;
int drink = 0;

//declare refill variables
float refill = 37.0; //distance to water-level when half full
float stopFill = 33.5;

//declare variables to keep track of time
int counter = 0;
unsigned long dayStart = 0;
float day_ms = 24 * 60 * 60 * 1000;


void setup() {
  //Initialize pins
  pinMode(motionPin, INPUT);
  pinMode(servoPin, OUTPUT);
  
  consumptionTotal = 0;
  drinkCount = 0;
  fillCount = 0;
  
  //set up motor and reference angle
  Servo1.attach(servoPin);
  Servo1.write(45);
  
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
  
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  
  //Check if it is the same day and reset variables if a new day and correct for when millis() resets at 49.7 days
   if (((millis() - dayStart) >= day_ms) && (millis() >= (0.3 * day_ms))){
    //Send consumption total by MQTT
    consumptionTotal = 0;
    drinkCount = 0;
    fillCount = 0;
    dayStart = millis();
  }
  
  
  
  //reset consumption to 0
  consumption = 0;

  //DISTANCE CALCULATION - Repeat every 5 minutes with no motion//
  int iterations = 15;
  duration = sonar.ping_median(iterations);
  distance = (duration / 2) * 0.0343;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  ArduinoCloud.update();
  
  //Check if water level is below the refill threshold and fill up
  if (distance > refill ) {
    Serial.print("Filling...");
    fillCount++;

    //TURN ON: Use Servo Motor to turn on water pump;
    // Make servo go to 0 degrees
    Servo1.write(0);
    delay(1000);
    // Make servo go back to 45 degrees
    Servo1.write(45);
    
    distanceCheck = distance;
    while (distanceCheck > stopFill) {
      //CHECK DISTANCE after filling
      int iterations = 5;
      duration = sonar.ping_median(iterations);
      distanceCheck = (duration / 2) * 0.0343;
      delay(100);
    }
    
    //TURN OFF: Use Servo Motor to turn on water pump;
    // Make servo go to 0 degrees
    Servo1.write(0);
    delay(1000);
    // Make servo go back to 45 degrees
    Servo1.write(45);

    delay (10000);
    //CHECK DISTANCE after filling
    int iterations = 15;
    duration = sonar.ping_median(iterations);
    distance = (duration / 2) * 0.0343;
    ArduinoCloud.update();
  }



  drink = 0;
  counter = 0;
  //monitor motion for 5 minutes or until detected
  while (counter < 300) { //300 * 1000 corresponds to approximately 5 minutes
    delay(1000);
    motion = digitalRead(motionPin);
    if (motion == HIGH) {
      drink = 1;    //detect if dog is taking a drink
      counter = 300;     //end loop
      Serial.println("Drinking...");
    }
    Serial.println("Checking motion");
    counter++;
  }
  
  //wait for motion to be stopped for 5 minutes and then calculate consumption
  counter = 0;
  while (drink == 1 && counter < 300) {
    delay(1000);
    motion = digitalRead(motionPin);
    if (motion == HIGH) {
      counter = 0;
    }
    counter++;
  }

  if (drink == 1) {
    //DISTANCE CALCULATION - after drinking//
    int iterations = 15;
    duration = sonar.ping_median(iterations);
    distanceNew = (duration / 2) * 0.0343;
    consumption = area * (distanceNew - distance);
    consumptionTotal = consumptionTotal + consumption;
    drinkCount = drinkCount++;
    distance = distanceNew;
    // Displays the consumption on the Serial Monitor
    Serial.print("Consumption: ");
    Serial.print(consumption);
    Serial.println(" mL");
    ArduinoCloud.update();
  }
}





