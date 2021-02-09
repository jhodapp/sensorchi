#include "pressure_sensor.h"

PressureSensor::PressureSensor()
{
    BaseSensor::setup();
}

PressureSensor::~PressureSensor()
{
}

float PressureSensor::read(bool mock_data)
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
        lastValue = BaseSensor::sensor().readPressure() / 100.0F;
    }

    return lastValue;
}