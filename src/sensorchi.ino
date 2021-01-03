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

#include "JsonParserGeneratorRK.h"
#include "Particle.h"
#include "SeeedOLED.h"

#define USE_JSON_PAYLOAD

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
//BleLogging<4096> bleLogHandler(LOG_LEVEL_INFO);

SerialLogHandler serialLogHandler(LOG_LEVEL_INFO);

TCPClient client;

const unsigned long PUSH_SENSOR_DATA_INTERVAL = 5000; // milliseconds
unsigned long lastPushSensorData = 0;
float temp = 0, humidity = 0, dust_concentration = 0;
int pressure = 0;
String air_purity;

static void DisplayOledSplash();
static void DisplayOledReadings();
static void LogReadings();
static void ReadSensors();
static void CreateAndSendEventPayload(float temp, float humidity, int pressure, float dust_concentration, const String &air_purity);
static void CreateAndSendSensorsLocal();

// setup() runs once, when the device is first turned on.
void setup() {
    // I2C bus init for use with OLED display
    Wire.begin();
    SeeedOled.init();
    SeeedOled.clearDisplay();
    SeeedOled.setNormalDisplay();
    SeeedOled.setPageMode();

    Log.info("Setup complete.");
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    if (millis() - lastPushSensorData >= PUSH_SENSOR_DATA_INTERVAL) {
        lastPushSensorData = millis();

        ReadSensors();
        DisplayOledReadings();
        LogReadings();
        CreateAndSendSensorsLocal();
    }

    if (client.available()) {
        const char c = client.read();
        Serial.print(c);
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
  SeeedOled.putString("Dust: ");
  SeeedOled.putNumber(dust_concentration);
  SeeedOled.putString(" pcs/L");

  SeeedOled.setTextXY(5, 0);
  SeeedOled.putString("                ");
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

static void CreateAndSendEventPayload(float temp, float humidity, int pressure, float dust_concentration, const String &air_purity)
{
    JsonWriterStatic<256> jw;

    {
        JsonWriterAutoObject obj(&jw);

        jw.insertKeyValue("temp", temp);
        jw.insertKeyValue("humidity", humidity);
        jw.insertKeyValue("pressure", pressure);
        jw.insertKeyValue("dust-concentration", dust_concentration);
        jw.insertKeyValue("air-purity", air_purity);
    }

    Particle.publish("sensor-readings", jw.getBuffer(), PRIVATE);
}

static void CreateAndSendSensorsLocal()
{
    const byte server[] = { 10, 0, 1, 13 };

    if (client.connect(server, 4000))
    {
        JsonWriterStatic<256> jw;
        {
            JsonWriterAutoObject obj(&jw);
            jw.insertKeyValue("temp", temp);
            jw.insertKeyValue("humidity", humidity);
        }

        Log.info("TCPClient connected");
        Log.info("ip address: %s", WiFi.localIP().toString().c_str());
        Log.info("gateway: %s", WiFi.gatewayIP().toString().c_str());
#ifdef USE_JSON_PAYLOAD
        Log.info("Sending POST request:");
        Log.info("POST /sensors HTTP/1.1");
        Log.info("User-Agent: argon/0.0.1");
        Log.info("Accept: */*");
        Log.info("Content-Type: application/json");
        Log.info("Content-Length: %d", 39);
        //Log.info("Content-Length: %d", jw.getBufferLen());
        Log.info(jw.getBuffer());


        client.print("POST /api/sensors HTTP/1.1\r\n");
        client.print("Host: 10.0.1.13:4000\r\n");
        client.print("User-Agent: argon/0.0.1\r\n");
        client.print("Accept: */*\r\n");
        client.print("Content-Type: application/json\r\n");
        client.print("Content-Length: ");
        client.print(39);
        //client.print(jw.getBufferLen());
        client.print("\r\n");
        client.print("\r\n");
        client.print(jw.getBuffer());
        client.print("\r\n");
#else
        Log.info("Sending GET request:");
        //Log.info("GET /hello/Argon HTTP/1.1");
        Log.info("GET /sensors?temp=%f;humidity=%f HTTP/1.1", temp, humidity);
        Log.info("Host: 10.0.1.13:4000");
        Log.info("User-Agent: argon");
        Log.info("Accept: */*");
        Log.info("Content-Length: 0");

        //client.print("GET /hello/argon HTTP/1.1\r\n");
        client.print("GET /sensors?temp=");
        client.print(temp, 10);
        client.print("&humidity=");
        client.print(humidity, 10);
        client.print(" HTTP/1.1\r\n");
        client.print("Host: 10.0.1.13:4000\r\n");
        client.print("User-Agent: argon/0.0.1\r\n");
        client.print("Accept: */*\r\n");
        client.print("Content-Length: 0\r\n");
        client.print("\r\n");
#endif
    } else {
        Log.error("TCPClient could not connect to endpoint");
    }
}