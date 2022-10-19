#ifndef GAME_H
#define GAME_H

#include "Arduino.h"
#include "DigitalSensor.h"
#include "SteeringWheel.h"
#include "TurnSignal.h"
#include "Shifter.h"
#include "Pins.h"
enum State
{
    WAITING,
    START,
    GENERATE_ACTION,
    USER_INPUT,
    DONE
};

enum Action
{
    SHIFT0,
    SHIFT1,
    SHIFT2,
    SHIFT3,
    STEER_LEFT,
    STEER_RIGHT,
    HORN,
    TURN_LEFT,
    TURN_RIGHT,
    NO_TURN
};
class Game
{
private:
    DigitalSensor *start;
    DigitalSensor *horn;
    SteeringWheel *wheel;
    TurnSignal *turnSignal;
    Shifter *shifter;

    Action generateAction()
    {
        int r = random(0, 10);
        return static_cast<Action>(r);
    }

    Action getUserAction()
    {
        int shifterChange = shifter->detectChange();
        int turnSignalDir = turnSignal->detectChange();
        Direction wheelDirection = wheel->getDirection();
        if (horn->getReading() == 1)
        {
            return Action::HORN;
        }
        else if (shifterChange != -1)
        {
            return static_cast<Action>(shifterChange);
        }
        else if (wheelDirection != Direction::NONE)
        {
            return wheelDirection == Direction::LEFT ? Action::STEER_LEFT : Action::STEER_RIGHT;
        }
        else if (turnSignal != -1)
        {
            if (turnSignal == 0)
            {
                return Action::TURN_LEFT;
            }
            else if (turnSignal == 1)
            {
                return Action::NO_TURN;
            }
            else
            {
                return Action::TURN_RIGHT;
            }
        }
        else
            return NULL;
    }

public:
    Game()
    {
        start = new DigitalSensor(START_PIN);
        horn = new DigitalSensor(HORN_PIN);
        wheel = new SteeringWheel;
        shifter = new Shifter;
        turnSignal = new TurnSignal;
    }
    ~Game()
    {
        delete start;
        delete horn;
        delete wheel;
        delete shifter;
        delete turnSignal;
    }
    void runGame()
    {
        State state = State::WAITING;
        int totalAttempts;
        int correctAttempts;
        unsigned long timeToAction = 5000;
        Action generatedAction;
        while (true)
        {
            if (state == State::WAITING)
            {
                if (start->getReading() == 1)
                {
                    state = State::START;
                }
            }
            else if (state == State::START)
            {
                totalAttempts = 0;
                correctAttempts = 0;
                state = State::GENERATE_ACTION;
            }
            else if (state == State::GENERATE_ACTION)
            {
                generatedAction = generateAction();
                state = State::USER_INPUT;
            }
            else if (state == State::USER_INPUT)
            {
                bool actionMade = false;
                unsigned long startingTime = millis();
                while (millis() < startingTime + timeToAction)
                {
                    if (getUserAction() == generatedAction)
                    {
                        actionMade = true;
                        correctAttempts++;
                        timeToAction -= 100;
                        break;
                    }
                    else if (getUserAction())
                    {
                        // Screen shows unsuccessful attempt
                        actionMade = true;
                        break;
                    }
                }
                totalAttempts++;
                if (correctAttempts == 99 || !actionMade)
                    state = State::DONE;
                else
                    state = State::GENERATE_ACTION;
            }
            else if (state == State::DONE)
            {
                // Screen shows correctAttempts/totalAttempts
                state = State::WAITING;
            }
        }
    }
};
#endif
