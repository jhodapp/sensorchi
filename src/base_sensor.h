#ifndef BASE_SENSOR_H
#define BASE_SENSOR_H

#undef max
#include <vector>

#include "Adafruit_BME280.h"

#include "Particle.h"

class BaseSensor
{
    public:
        BaseSensor();
        virtual ~BaseSensor();

        virtual bool setup();
        // TODO: make read() be container-based so it can return any type
        // and get rid of read_str()
        virtual float read(bool mock_data = false);
        virtual String read_str(bool mock_data = false);

    protected:
        virtual Adafruit_BME280 & sensor();

    private:
        bool doOnce;
        Adafruit_BME280 bme_sensor;
};

#endif