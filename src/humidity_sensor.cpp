#include "humidity_sensor.h"

HumiditySensor::HumiditySensor()
{
    BaseSensor::setup();
}

HumiditySensor::~HumiditySensor()
{
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
        lastValue = BaseSensor::sensor().readHumidity();
    }

    return lastValue;
}