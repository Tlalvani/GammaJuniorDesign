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
        DigitalSensor temp[3] = {DigitalSensor(LEFT_TURN_PIN), DigitalSensor(NO_TURN_PIN), DigitalSensor(RIGHT_TURN_PIN)};
        for (int i = 0; i < 3; i++)
            turnSignal[i] = &temp[i];
        for (int i = 0; i < 3; i++)
        {
            prevVal[i] = turnSignal[i]->getReading();
        }
        Serial.println("PREVIOUS: " + String(prevVal[0]) + " " + String(prevVal[1]) + " " + String(prevVal[2]));
    }

    int detectChange()
    {
//        Serial.println("PREVIOUS: " + String(prevVal[0]) + " " + String(prevVal[1]) + " " + String(prevVal[2]));
//        Serial.println("CURRENT: " + String(turnSignal[0]->getReading()) + " " + String(turnSignal[1]->getReading()) + " " + String(turnSignal[2]->getReading()));
//        
        int newTurn = -1;
        for (int i = 0; i < 3; i++)
        {
            int val = turnSignal[i]->getReading();
            if (prevVal[i] != val)
            {
                Serial.println("DIFF: " + String(prevVal[i]) + " " + String(val));
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
