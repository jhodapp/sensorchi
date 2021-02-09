#include "base_sensor.h"

BaseSensor::BaseSensor()
 : doOnce(true)
{
}

BaseSensor::~BaseSensor()
{
}

bool BaseSensor::setup()
{
    if (doOnce) {
        doOnce = false;
        // Initialize the BME280 sensor
        return bme_sensor.begin();
    }

    return true;
}

float BaseSensor::read(bool mock_data)
{
    return 0.0;
}

String BaseSensor::read_str(bool mock_data)
{
    return "BaseSensor read_str()";
}

Adafruit_BME280 & BaseSensor::sensor()
{
    return bme_sensor;
}