/*
 * Project sensorchi
 * Description:
 * Author: Jim Hodapp
 * Date: 12/2/20
 */

#include "ble_logger.h"
#include "humidity_sensor.h"
#include "temperature_sensor.h"

#include "Particle.h"

// This firmware works better with system thread enabled, otherwise it is not
// initialized until you've already connected to the cloud, which is not as useful.
SYSTEM_THREAD(ENABLED);

HumiditySensor humidity_sensor;
TemperatureSensor temp_sensor;

// This sets up the BLE log handler. The <4096> template parameter sets the size of the buffer to hold log data
// The other parameters are like SerialLogHandler. You can set the log level (optional) to things like
// LOG_LEVEL_ALL, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, etc.. You can also pass a log filter here.
BleLogging<4096> bleLogHandler(LOG_LEVEL_INFO);

// Optionally you can also enable USB serial log handling (or other log handlers, as desired).
//SerialLogHandler serialLogHandler(LOG_LEVEL_TRACE);

// This is just so the demo prints a message every second so the log updates frequently
const unsigned long LOG_INTERVAL = 5000; // milliseconds
unsigned long lastLog = 0;
float temp = 0, humidity = 0;

// setup() runs once, when the device is first turned on.
void setup() {
    BLE.on();

    bleLogHandler.setup();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    bleLogHandler.loop();

    if (millis() - lastLog >= LOG_INTERVAL) {
        lastLog = millis();

        temp_sensor.setTemperatureUnits(TemperatureSensor::TemperatureUnits::Celcius);
        temp = temp_sensor.read();
        Log.info("t = %0.0fC", temp);

        temp_sensor.setTemperatureUnits(TemperatureSensor::TemperatureUnits::Fahrenheit);
        temp = temp_sensor.read();
        Log.info("t = %0.1fF", temp);

        temp_sensor.setTemperatureUnits(TemperatureSensor::TemperatureUnits::Kelvin);
        temp = temp_sensor.read();
        Log.info("t = %0.2fK", temp);

        humidity = humidity_sensor.read();
        Log.info("h = %0.1f percent", humidity);
    }
}