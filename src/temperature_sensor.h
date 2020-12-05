#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "base_sensor.h"

#include "Adafruit_BME280.h"

class TemperatureSensor : public BaseSensor
{
    public:
        enum TemperatureUnits { Celcius, Fahrenheit, Kelvin };
        TemperatureSensor();
        ~TemperatureSensor();

        virtual bool setup();
        virtual float read(bool mock_data = false);

        void setTemperatureUnits(TemperatureUnits units);
        TemperatureUnits getTemperatureUnits() const;

    private:
        Adafruit_BME280 sensor;

        TemperatureUnits temperature_units;

        float convertUnits(float temp);
};

#endif