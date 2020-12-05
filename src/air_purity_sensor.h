#ifndef AIR_PURITY_SENSOR_H
#define AIR_PURITY_SENSOR_H

#include "base_sensor.h"

#include "Air_Quality_Sensor.h"

#define AQS_PIN A2

class AirPuritySensor : public BaseSensor
{
    public:
        AirPuritySensor();
        ~AirPuritySensor();

        virtual bool setup();
        virtual String read_str(bool mock_data = false);

    private:
        AirQualitySensor *sensor;
};

#endif