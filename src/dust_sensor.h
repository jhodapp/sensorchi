#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "base_sensor.h"

class DustSensor : public BaseSensor
{
    public:
        DustSensor();
        ~DustSensor();

        virtual bool setup();
        virtual float read(bool mock_data = false);

    private:
        unsigned long lastInterval;
        unsigned long lowPulseOccupancy;
        unsigned long lastLpo;
        unsigned long duration;

        float ratio = 0;
        float concentration = 0;

        void getSensorValues();
};

#endif