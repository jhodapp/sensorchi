#include "dust_sensor.h"

#include "Particle.h"

#include <math.h>

#define DUST_SENSOR_PIN D4
#define SENSOR_READING_INTERVAL 30000

DustSensor::DustSensor()
    : lastInterval(0),
      lowPulseOccupancy(0),
      lastLpo(0),
      duration(0),
      ratio(0),
      concentration(0)
{
    setup();
}

DustSensor::~DustSensor()
{
}

bool DustSensor::setup()
{
    static bool doOnce = true;

    if (doOnce) {
        doOnce = false;

        // Connect to the Shinyei PPD42 dust sensor
        Serial.begin(9600);

        pinMode(DUST_SENSOR_PIN, INPUT);
        lastInterval = millis();

        return true;
    }

    return false;
}

float DustSensor::read(bool mock_data)
{
    if (mock_data) {
        // Adjust this by a little bit each check so we can see it change
        if (rand() > (RAND_MAX / 2)) {
            concentration += 1.1;
        }
        else {
            concentration -= 1.1;
        }
    } else {
        duration = pulseIn(DUST_SENSOR_PIN, LOW);
        lowPulseOccupancy = lowPulseOccupancy + duration;

        getSensorValues();

        lowPulseOccupancy = 0;
    }

    return concentration;
}

void DustSensor::getSensorValues()
{
    // This particular dust sensor returns 0s often, so let's filter them out by making sure we only
    // capture and use non-zero LPO values for our calculations once we get a good reading.
    if (lowPulseOccupancy == 0) {
        lowPulseOccupancy = lastLpo;
    } else {
        lastLpo = lowPulseOccupancy;
    }

    ratio = lowPulseOccupancy / (SENSOR_READING_INTERVAL * 10.0);                   // Integer percentage 0=>100
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
}