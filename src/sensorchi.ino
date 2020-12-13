/*
 * Project sensorchi
 * Description:
 * Author: Jim Hodapp
 * Date: 12/2/20
 */

#include "air_purity_sensor.h"
#include "ble_logger.h"
#include "dust_sensor.h"
#include "humidity_sensor.h"
#include "temperature_sensor.h"

#include "Particle.h"
#include "SeeedOLED.h"

// This firmware works better with system thread enabled, otherwise it is not
// initialized until you've already connected to the cloud, which is not as useful.
SYSTEM_THREAD(ENABLED);

AirPuritySensor air_purity_sensor;
DustSensor dust_sensor;
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
float temp = 0, humidity = 0, dust_concentration = 0;
String air_purity;

static void DisplayOledSplash();
static void DisplayOledReadings();
static void LogReadings();
static void ReadSensors();

// setup() runs once, when the device is first turned on.
void setup() {
    BLE.on();
    bleLogHandler.setup();

    // I2C bus init for use with OLED display
    Wire.begin();
    SeeedOled.init();
    SeeedOled.clearDisplay();
    SeeedOled.setNormalDisplay();
    SeeedOled.setPageMode();

    DisplayOledSplash();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    bleLogHandler.loop();

    if (millis() - lastLog >= LOG_INTERVAL) {
        lastLog = millis();

        ReadSensors();
        LogReadings();
        DisplayOledReadings();
    }
}

static void DisplayOledSplash()
{
    SeeedOled.setTextXY(3, 0);
    SeeedOled.putString("Air Quality");
    SeeedOled.setTextXY(4, 0);
    SeeedOled.putString("Monitor");
}

static void DisplayOledReadings()
{
  SeeedOled.clearDisplay();

  SeeedOled.setTextXY(1, 0);
  SeeedOled.putString("Temp: ");
  SeeedOled.putNumber(temp);
  SeeedOled.putString("C");

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Humidity: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");

  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("Dust: ");
  SeeedOled.putNumber(dust_concentration);
  SeeedOled.putString(" pcs/L");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString(air_purity);
}

static void LogReadings()
{
    Log.info("temp: %0.0fC", temp);
    Log.info("humidity: %0.1f percent", humidity);
    Log.info("dust: %0.1f pcs/L", dust_concentration);
    Log.info(air_purity.c_str());
}

static void ReadSensors()
{
    temp_sensor.setTemperatureUnits(TemperatureSensor::TemperatureUnits::Celcius);
    temp = temp_sensor.read();

    humidity = humidity_sensor.read();

    air_purity = air_purity_sensor.read_str();

    dust_concentration = dust_sensor.read();
}