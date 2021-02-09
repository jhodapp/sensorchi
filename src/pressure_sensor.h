#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include "base_sensor.h"

class PressureSensor : public BaseSensor
{
    public:
        PressureSensor();
        ~PressureSensor();

        virtual float read(bool mock_data = false);
};

#endif