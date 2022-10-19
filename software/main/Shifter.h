#ifndef SHIFTER_H
#define SHIFTER_H

#include "Arduino.h"
#include "DigitalSensor.h"
#include "Pins.h"

class Shifter
{
private:
    DigitalSensor *shifter[4];
    int currentGear;
    int prevVal[4];

public:
    Shifter()
    {
        DigitalSensor temp[4] = {DigitalSensor(SHIFT_0_PIN), DigitalSensor(SHIFT_1_PIN), DigitalSensor(SHIFT_2_PIN), DigitalSensor(SHIFT_3_PIN)};
        for (int i = 0; i < 4; i++)
            shifter[i] = &temp[i];
        for (int i = 0; i < 4; i++)
        {
            prevVal[i] = shifter[i]->getReading();
        }
    }

    int detectChange()
    {
        int newGearIdx = -1;
        for (int i = 0; i < 4; i++)
        {
            if (prevVal[i] != shifter[i]->getReading())
            {
                int val = shifter[i]->getReading();
                prevVal[i] = val;
                if (val == 0)
                {
                    newGearIdx = i;
                }
            }
        }
        return newGearIdx;
    }
};

#endif