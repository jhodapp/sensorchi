#include "air_purity_sensor.h"

#include "Particle.h"

AirPuritySensor::AirPuritySensor()
    : sensor(nullptr)
{
    setup();
}

AirPuritySensor::~AirPuritySensor()
{
    if (sensor != nullptr)
        delete sensor;
}

bool AirPuritySensor::setup()
{
    static bool doOnce = true;

    if (doOnce) {
        doOnce = false;

        sensor = new AirQualitySensor(AQS_PIN);
        // Initialize the air quality sensor
        return sensor->init();
    }

    return false;
}

String AirPuritySensor::read_str(bool mock_data)
{
    if (sensor == nullptr)
        return "Error reading air quality level";

    static int lastValue = 0;
    if (mock_data) {
        lastValue++;
        if (lastValue > 3)
            lastValue = 0;
    } else {
        // Read from the Grove Air Quality sensor
        lastValue = sensor->slope();
    }

    if (lastValue == AirQualitySensor::FORCE_SIGNAL)
        return "Dangerous Pollution";
    else if (lastValue == AirQualitySensor::HIGH_POLLUTION)
        return "High Pollution";
    else if (lastValue == AirQualitySensor::LOW_POLLUTION)
        return "Low Pollution";
    else if (lastValue == AirQualitySensor::FRESH_AIR)
        return "Fresh Air";
    else
        return "Unknown air quality level";
}