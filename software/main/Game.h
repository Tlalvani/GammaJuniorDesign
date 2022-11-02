#ifndef GAME_H
#define GAME_H

#include "Arduino.h"
#include "DigitalSensor.h"
#include "SteeringWheel.h"
#include "TurnSignal.h"
#include "Shifter.h"
#include "Display.h"
#include "Pins.h"
#include <TFT_HX8357.h> // Hardware-specific library

enum State
{
    WAITING,
    START,
    GENERATE_ACTION,
    USER_INPUT,
    DONE
};

const String actions[] = {"shift_0", "shift_1", "shift_2", "shift_3", "steer_left", "steer_right", "horn", "turn_left", "turn_right", "no_turn"};

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
    Display *display;
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
        if (horn->getReading() == 0)
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
            return -1;
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
        delete display;
    }
    void runGame()
    {
        Serial.println("Game Running");
        State state = State::START;
        int totalAttempts;
        int correctAttempts;
        unsigned long timeToAction = 5000;
        Action generatedAction;
        while (true)
        {
            if (state == State::WAITING)
            {
                display->print("Waiting for start");
                Serial.println("Waiting for start");
                if (start->getReading() == 1)
                {
                    Serial.println("Started");
                    state = State::START;
                }
            }
            else if (state == State::START)
            {
                display->print("Starting");
                totalAttempts = 0;
                correctAttempts = 0;
                state = State::GENERATE_ACTION;
            }
            else if (state == State::GENERATE_ACTION)
            {
                generatedAction = generateAction();
                state = State::USER_INPUT;
                delay(2000);
                display->print("Generated Action: " + actions[generatedAction]);
                Serial.print("Generated Action: ");
                Serial.println(actions[generatedAction]);
            }
            else if (state == State::USER_INPUT)
            {
                bool actionMade = false;
                unsigned long startingTime = millis();
                Action action = -1;
                while (millis() < startingTime + timeToAction)
                {
                    action = getUserAction();
                    if (action == generatedAction)
                    {
                        actionMade = true;
                        correctAttempts++;
                        timeToAction -= 100;
                        break;
                    }
                    else if (action != -1)
                    {
                        // Screen shows unsuccessful attempt
                        actionMade = true;
                        break;
                    }
                }
                if (action)
                {
                    display->print("Chosen Action: " + actions[action]);
                    Serial.print("Chosen Action: ");
                    Serial.println(actions[action]);
                }
                else
                {
                    display->print("No action!");
                }
                totalAttempts++;
                if (correctAttempts == 99 || !actionMade)
                {
                    state = State::DONE;
                }
                else
                {
                    state = State::GENERATE_ACTION;
                }
            }
            else if (state == State::DONE)
            {
                // Screen shows correctAttempts/totalAttempts
                display->print("Score: " + String(correctAttempts / totalAttempts));
                delay(5000);
                state = State::WAITING;
            }
        }
    }
};
#endif
