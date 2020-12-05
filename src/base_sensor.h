#ifndef BASE_SENSOR_H
#define BASE_SENSOR_H

class BaseSensor
{
    public:
        BaseSensor();
        virtual ~BaseSensor();

        virtual bool setup() = 0;
        virtual float read(bool mock_data = false) = 0;

    private:

};

#endif