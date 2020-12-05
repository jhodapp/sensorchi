#include "humidity_sensor.h"

#include "Particle.h"

HumiditySensor::HumiditySensor()
{
    setup();
}

HumiditySensor::~HumiditySensor()
{
}

bool HumiditySensor::setup()
{
    static bool doOnce = true;

    if (doOnce) {
        // Initialize the BME280 sensor
        return sensor.begin();
    }

    return false;
}

float HumiditySensor::read(bool mock_data)
{
    static float lastValue = 0;
    if (mock_data) {
        // Adjust this by a little bit each check so we can see it change
        if (rand() > (RAND_MAX / 2)) {
            lastValue += 5.1;
        }
        else {
            lastValue -= 5.1;
        }
    } else {
        // Read from the BME280 sensor
        lastValue = sensor.readHumidity();
    }

    return lastValue;
}