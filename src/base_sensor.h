#ifndef BASE_SENSOR_H
#define BASE_SENSOR_H

#include "Particle.h"

class BaseSensor
{
    public:
        BaseSensor();
        virtual ~BaseSensor();

        virtual bool setup() = 0;
        virtual float read(bool mock_data = false);
        virtual String read_str(bool mock_data = false);

    private:

};

#endif