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
    TurnSignal()
    {
        //        DigitalSensor temp[3] = {DigitalSensor(LEFT_TURN_PIN), DigitalSensor(NO_TURN_PIN), DigitalSensor(RIGHT_TURN_PIN)};
        //        for (int i = 0; i < 3; i++)
        //            turnSignal[i] = &temp[i];
        //        for (int i = 0; i < 3; i++)
        //        {
        //            prevVal[i] = turnSignal[i]->getReading();
        //        }
        //        Serial.println("PREVIOUS: " + String(prevVal[0]) + " " + String(prevVal[1]) + " " + String(prevVal[2]));
        int tmp[3] = {digitalRead(LEFT_TURN_PIN), digitalRead(NO_TURN_PIN), digitalRead(RIGHT_TURN_PIN)};

        for (int i = 0; i < 3; i++)
            prevVal[i] = tmp[i];
    }

    int detectChange()
    {
        int newTurn = -1;
        int curr[3] = {digitalRead(LEFT_TURN_PIN), digitalRead(NO_TURN_PIN), digitalRead(RIGHT_TURN_PIN)};
        for (int i = 0; i < 3; i++)
        {
            int val = curr[i];
            if (prevVal[i] != val)
            {
                prevVal[i] = val;
                if (val == 0)
                {
                    newTurn = i;
                }
            }
        }
        return newTurn;
    }
};

#endif
