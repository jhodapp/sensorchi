#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "base_sensor.h"

class HumiditySensor : public BaseSensor
{
    public:
        HumiditySensor();
        ~HumiditySensor();

        virtual float read(bool mock_data = false);
};

#endif