#include "base_sensor.h"

BaseSensor::BaseSensor()
{

}

BaseSensor::~BaseSensor()
{
    
}

float BaseSensor::read(bool mock_data)
{
    return 0.0;
}

String BaseSensor::read_str(bool mock_data)
{
    return "BaseSensor read_str()";
}