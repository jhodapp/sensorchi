/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/home/jhodapp/Projects/particle/sensorchi/src/sensorchi.ino"
/*
 * Project sensorchi
 * Description:
 * Author: Jim Hodapp
 * Date: 12/2/20
 */

#include "temperature_sensor.h"

// This firmware works better with system thread enabled, otherwise it is not
// initialized until you've already connected to the cloud, which is not as useful.
void setup();
void loop();
#line 12 "/home/jhodapp/Projects/particle/sensorchi/src/sensorchi.ino"
SYSTEM_THREAD(ENABLED);

TemperatureSensor temp_sensor;

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.


}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

}