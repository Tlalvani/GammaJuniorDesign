#ifndef TURNSIGNAL_H
#define TURNSIGNAL_H

#include "Arduino.h"
#include "DigitalSensor.h"
#include "Pins.h"

class TurnSignal
{
private:
    DigitalSensor *turnSignal[3];
    int prevVal[3];

public:
    Shifter()
    {
        DigitalSensor temp[3] = {DigitalSensor(LEFT_TURN_PIN), DigitalSensor(NO_TURN_PIN), DigitalSensor(RIGHT_TURN_PIN)};
        for (int i = 0; i < 4; i++)
            turnSignal[i] = &temp[i];
        for (int i = 0; i < 4; i++)
        {
            prevVal[i] = turnSignal[i]->getReading();
        }
    }

    int detectChange()
    {
        for (int i = 0; i < 3; i++)
        {
            if (prevVal[i] != turnSignal[i]->getReading())
            {
                prevVal[i] = turnSignal[i]->getReading();
                if (turnSignal[i] == 0)
                {
                    return i;
                }
            }
        }
        return -1;
    }
};

#endif
