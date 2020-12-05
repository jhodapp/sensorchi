#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include "base_sensor.h"

#include "Adafruit_BME280.h"

class HumiditySensor : public BaseSensor
{
    public:
        HumiditySensor();
        ~HumiditySensor();

        virtual bool setup();
        virtual float read(bool mock_data = false);

    private:
        Adafruit_BME280 sensor;
};

#endif