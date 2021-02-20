/*
 * Project sensorchi
 * Description:
 * Author: Jim Hodapp
 * Date: 12/2/20
 */

#include "air_purity_sensor.h"
#include "dust_sensor.h"
#include "http_client.h"
#include "humidity_sensor.h"
#include "pressure_sensor.h"
#include "temperature_sensor.h"

#include "JsonParserGeneratorRK.h"
#include "Particle.h"
#include "SeeedOLED.h"

// This firmware works better with system thread enabled, otherwise it is not
// initialized until you've already connected to the cloud, which is not as useful.
SYSTEM_THREAD(ENABLED);

AirPuritySensor air_purity_sensor;
DustSensor dust_sensor;
HumiditySensor humidity_sensor;
PressureSensor pressure_sensor;
TemperatureSensor temp_sensor;

SerialLogHandler serialLogHandler(LOG_LEVEL_INFO);

const byte dev_server[] = { 10, 0, 1, 3 };
const byte prod_server[] = { 10, 0, 1, 2 };

HttpClient http_client_dev(dev_server, 4000);
HttpClient http_client_prod(prod_server, 4000);

const unsigned long PUSH_SENSOR_DATA_INTERVAL = 5000; // milliseconds
unsigned long lastPushSensorData = 0;
float temp = 0, humidity = 0, dust_concentration = 0;
int pressure = 0;
String air_purity;

static void DisplayOledReadings();
static void LogReadings();
static void ReadSensors();
static void CreateAndSendSensorPayload();

// setup() runs once, when the device is first turned on.
void setup() {
    // I2C bus init for use with OLED display
    Wire.begin();
    SeeedOled.init();
    SeeedOled.clearDisplay();
    SeeedOled.setNormalDisplay();
    SeeedOled.setPageMode();

    http_client_dev.setEnpoint("/api/readings/add");
    http_client_prod.setEnpoint("/api/readings/add");

    while (!WiFi.ready());
    http_client_dev.connect();
    http_client_prod.connect();

    Log.info("Setup complete.");
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    if (millis() - lastPushSensorData >= PUSH_SENSOR_DATA_INTERVAL) {
        lastPushSensorData = millis();

        ReadSensors();
        DisplayOledReadings();
        LogReadings();
        CreateAndSendSensorPayload();
    }

    if (http_client_dev.available()) {
        const char c = http_client_dev.read();
        Serial.print(c);
    }

    if (http_client_prod.available()) {
        const char c = http_client_prod.read();
        Serial.print(c);
    }
}

static void DisplayOledReadings()
{
  SeeedOled.setTextXY(1, 0);
  SeeedOled.putString("                ");
  SeeedOled.putString("Temp: ");
  SeeedOled.putNumber(temp);
  SeeedOled.putString("C");

  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("                ");
  SeeedOled.putString("Humidity: ");
  SeeedOled.putNumber(humidity);
  SeeedOled.putString("%");

  SeeedOled.setTextXY(3, 0);
  SeeedOled.putString("                ");
  SeeedOled.putString("Pressure: ");
  SeeedOled.putNumber(pressure);
  SeeedOled.putString(" mb");

  SeeedOled.setTextXY(4, 0);
  SeeedOled.putString("                ");
  SeeedOled.putString("Dust: ");
  SeeedOled.putNumber(dust_concentration);
  SeeedOled.putString(" pcs/L");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("                ");
  SeeedOled.putString(air_purity);
}

static void LogReadings()
{
    //Log.info("");
    Log.info("temp: %0.0fC", temp);
    Log.info("humidity: %0.1f percent", humidity);
    Log.info("pressure: %d mb", pressure);
    Log.info("dust: %0.1f pcs/L", dust_concentration);
    Log.info(air_purity.c_str());
}

static void ReadSensors()
{
    temp_sensor.setTemperatureUnits(TemperatureSensor::TemperatureUnits::Celcius);
    temp = temp_sensor.read();

    humidity = humidity_sensor.read();

    pressure = static_cast<int>(pressure_sensor.read());

    air_purity = air_purity_sensor.read_str();

    dust_concentration = dust_sensor.read();
}

static void CreateAndSendSensorPayload()
{
    if (!http_client_dev.connected())
        http_client_dev.connect();

    if (!http_client_prod.connected())
        http_client_prod.connect();

    if (http_client_dev.connected())
    {
        JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> jw;
        {
            JsonWriterAutoObject obj(&jw);
            jw.insertKeyValue("temperature", temp);
            jw.insertKeyValue("humidity", humidity);
            jw.insertKeyValue("pressure", pressure);
            jw.insertKeyValue("dust_concentration", dust_concentration);
            jw.insertKeyValue("air_purity", air_purity);
        }

        Log.info("ip address: %s", WiFi.localIP().toString().c_str());
        Log.info("gateway: %s", WiFi.gatewayIP().toString().c_str());

        http_client_dev.sendJson(jw);
    } else {
        Log.error("http_client_dev not connected to server");
    }

    if (http_client_prod.connected())
    {
        JsonWriterStatic<JSON_WRITER_BUFFER_SIZE> jw;
        {
            JsonWriterAutoObject obj(&jw);
            jw.insertKeyValue("temperature", temp);
            jw.insertKeyValue("humidity", humidity);
            jw.insertKeyValue("pressure", pressure);
            jw.insertKeyValue("dust_concentration", dust_concentration);
            jw.insertKeyValue("air_purity", air_purity);
        }

        Log.info("ip address: %s", WiFi.localIP().toString().c_str());
        Log.info("gateway: %s", WiFi.gatewayIP().toString().c_str());

        http_client_prod.sendJson(jw);
    } else {
        Log.error("http_client_prod not connected to server");
    }
}