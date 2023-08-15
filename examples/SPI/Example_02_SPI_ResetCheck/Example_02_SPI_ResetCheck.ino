/*
  Using the BNO08x IMU

  This example shows how check for a "Reset Complete" message from the sensor,
  which is helpful when used in tandem with requestResetReason() and
  getResetReason(). The sensor will be reset each time 100 readings are received
  to demonstrate.

  It outputs the x/y/z parts of the gyro.

  By: @mattbradford83
  Date: 1 August 2021

  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  SparkFun code, firmware, and software is released under the MIT License.
	Please see LICENSE.md for further details.

  Originally written by Nathan Seidle @ SparkFun Electronics, December 28th, 2017

  Adjusted by Pete Lewis @ SparkFun Electronics, June 2023 to incorporate the
  CEVA Sensor Hub Driver, found here:
  https://github.com/ceva-dsp/sh2

  Also, utilizing code from the Adafruit BNO08x Arduino Library by Bryan Siepert
  for Adafruit Industries. Found here:
  https://github.com/adafruit/Adafruit_BNO08x

  Also, utilizing I2C and SPI read/write functions and code from the Adafruit
  BusIO library found here:
  https://github.com/adafruit/Adafruit_BusIO

  Hardware Connections:
  IoT Readboard --> BNO08x
  D25  --> CS
  PICO --> SI
  POCI --> SO
  SCK  --> SCK
  D17  --> INT
  D16  --> RST
  3V3  --> 3V3
  GND  --> GND

  BNO08x "mode" pins set for SPI:
  PSO --> 3V3
  PS1 --> 3V3

  Serial.print it out at 115200 baud to serial monitor.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/22857
*/

#include "SparkFun_BNO08x_Arduino_Library.h"  // Click here to get the library: http://librarymanager/All#SparkFun_BNO08x
BNO08x myIMU;

// For SPI, we need some extra pins defined:
// Note, these can be other GPIO if you like.
#define BNO08X_CS   25
#define BNO08X_INT  17
#define BNO08X_RST  16

int cyclecount = 0;

#define BNO08X_ADDR 0x4B  // SparkFun BNO080 Breakout (Qwiic) defaults to 0x4B
//#define BNO08X_ADDR 0x4A // Alternate address if ADR jumper is closed

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("BNO08x Read Example");

  Wire.begin();

  if (myIMU.beginSPI(BNO08X_CS, BNO08X_INT, BNO08X_RST) == false) {
    Serial.println("BNO08x not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
    while (1)
      ;
  }
  Serial.println("BNO08x found!");

  if( myIMU.requestResetReason() == true ) {
      Serial.print(F("Reset Reason: "));
      printResetReasonName(myIMU.getResetReason());
      Serial.println();
    }
  else {
    Serial.println("Reset Reason Request failed.");
  }

  setReports();

  Serial.println("Reading events");
  delay(100);
}

// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (myIMU.enableGyro() == true) {
    Serial.println(F("Gyro enabled"));
    Serial.println(F("Output in form x, y, z, in radians per second"));
  } else {
    Serial.println("Could not enable gyro");
  }
}

void loop() {
  delay(1);
  myIMU.serviceBus();

  // One of these will appear at the very start because of the power on reset.
  // Use requestResetReason() and getResetReason() for the difference between
  // different resets.
  if (myIMU.wasReset()) {
    Serial.println(" ------------------ BNO08x has reset. ------------------ ");
    if( myIMU.requestResetReason() == true ) {
        Serial.print(F("Reset Reason: "));
        printResetReasonName(myIMU.getResetReason());
        Serial.println();
      }
    else {
      Serial.println("Reset Reason Request failed.");
    }

    setReports();  // We'll need to re-enable reports after any reset.
  }

  // Has a new event come in on the Sensor Hub Bus?
  if (myIMU.getSensorEvent() == true) {

    // is it the correct sensor data we want?
    if (myIMU.getSensorEventID() == SENSOR_REPORTID_GYROSCOPE_CALIBRATED) {

      cyclecount++;

      Serial.print(F("["));
      if (cyclecount < 10) Serial.print(F("0"));
      Serial.print(cyclecount);
      Serial.print(F("] "));

      float x = myIMU.getGyroX();
      float y = myIMU.getGyroY();
      float z = myIMU.getGyroZ();

      Serial.print(x, 2);
      Serial.print(F(","));
      Serial.print(y, 2);
      Serial.print(F(","));
      Serial.print(z, 2);
      Serial.print(F(","));

      Serial.println();

      if (cyclecount == 100) {
        myIMU.softReset();
        cyclecount = 0;
      }
    }
  }
}


//Given a number between 0 and 5, print the name of the reset reason
//1 = POR, 2 = Internal reset, 3 = Watchdog, 4 = External reset, 5 = Other
void printResetReasonName(byte resetReasonNumber)
{
  if(resetReasonNumber == 1) Serial.print("POR");
  else if(resetReasonNumber == 2) Serial.print("Internal reset");
  else if(resetReasonNumber == 3) Serial.print("Watchdog");
  else if(resetReasonNumber == 4) Serial.print("External reset");
  else if(resetReasonNumber == 5) Serial.print("Other");
}
