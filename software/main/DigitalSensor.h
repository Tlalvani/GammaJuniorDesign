#ifndef DIGITALSENSOR_H
#define DIGITALSENSOR_H

#include "Arduino.h"
class DigitalSensor
{
private:
    int pin;

public:
    DigitalSensor(int pin)
    {
        this->pin = pin;
    }

    int getReading()
    {
        return digitalRead(pin);
    }
};
#endif
