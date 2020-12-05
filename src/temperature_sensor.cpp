#include "temperature_sensor.h"

#include "Particle.h"

TemperatureSensor::TemperatureSensor()
    : temperature_units(TemperatureUnits::Celcius)
{
    setup();
}

TemperatureSensor::~TemperatureSensor()
{
}

bool TemperatureSensor::setup()
{
    static bool doOnce = true;

    if (doOnce) {
        // Initialize the BME280 sensor
        return sensor.begin();
    }

    return false;
}

float TemperatureSensor::read(bool mock_data)
{
    static float lastValue = 0;
    if (mock_data) {
        // Adjust this by a little bit each check so we can see it change
        if (rand() > (RAND_MAX / 2)) {
            lastValue += 0.1;
        }
        else {
            lastValue -= 0.1;
        }
    } else {
        // Read from the BME280 sensor
        lastValue = sensor.readTemperature();
    }

    return convertUnits(lastValue);
}

void TemperatureSensor::setTemperatureUnits(TemperatureSensor::TemperatureUnits units)
{
    temperature_units = units;
}

TemperatureSensor::TemperatureUnits TemperatureSensor::getTemperatureUnits() const
{
    return temperature_units;
}

float TemperatureSensor::convertUnits(float temp)
{
    switch (temperature_units)
    {
        case TemperatureUnits::Fahrenheit:
            return ((temp * 9.0) / 5.0) + 32.0;
        case TemperatureUnits::Kelvin:
            return temp + 273.15;
        case TemperatureUnits::Celcius:
        default:
            return temp;
    }
}