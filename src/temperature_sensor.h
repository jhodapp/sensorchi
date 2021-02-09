#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "base_sensor.h"

class TemperatureSensor : public BaseSensor
{
    public:
        enum TemperatureUnits { Celcius, Fahrenheit, Kelvin };
        TemperatureSensor();
        ~TemperatureSensor();

        virtual float read(bool mock_data = false);

        void setTemperatureUnits(TemperatureUnits units);
        TemperatureUnits getTemperatureUnits() const;

    private:
        TemperatureUnits temperature_units;

        float convertUnits(float temp);
};

#endif