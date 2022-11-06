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
//        DigitalSensor temp[4] = {DigitalSensor(SHIFT_0_PIN), DigitalSensor(SHIFT_3_PIN), DigitalSensor(SHIFT_2_PIN), DigitalSensor(SHIFT_3_PIN)};
//        for (int i = 0; i < 4; i++)
//            shifter[i] = &temp[i];
//        for (int i = 0; i < 4; i++)
//        {
//            prevVal[i] = shiffter[i]->getReading();
//        }
         int tmp[4] = {digitalRead(SHIFT_0_PIN), digitalRead(SHIFT_1_PIN), digitalRead(SHIFT_2_PIN), digitalRead(SHIFT_3_PIN)};
         
         for (int i = 0; i < 4; i++)
            prevVal[i] = tmp[i];
    }

    int detectChange()
    {
        int newGearIdx = -1;
        int curr[4] = {digitalRead(SHIFT_0_PIN), digitalRead(SHIFT_3_PIN), digitalRead(SHIFT_2_PIN), digitalRead(SHIFT_3_PIN)};
        for (int i = 0; i < 4; i++)
        {
            int val = curr[i];
            if (prevVal[i] != val)
            {
                Serial.println("DIFF: " + String(prevVal[i]) + " " + String(val));
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
