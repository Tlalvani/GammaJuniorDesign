#ifndef GAME_H
#define GAME_H

#include "Arduino.h"
#include "DigitalSensor.h"
#include "SteeringWheel.h"
#include "TurnSignal.h"
#include "Shifter.h"
#include "Display.h"
#include "Pins.h"

enum State
{
    WAITING,
    START,
    GENERATE_ACTION,
    USER_INPUT,
    DONE
};

const String actions[] = {"shift", "steer_left", "steer_right", "horn", "turn_signal"};

enum Action
{
    SHIFT,
    STEER_LEFT,
    STEER_RIGHT,
    HORN,
    TURN_SIGNAL
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
        int r = random(0, 5);
        return static_cast<Action>(r);
    }


public:
    Game()
    {
        start = new DigitalSensor(START_PIN);
        horn = new DigitalSensor(HORN_PIN);
        wheel = new SteeringWheel;
        shifter = new Shifter;
        turnSignal = new TurnSignal;
        display = new Display;
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

    Action getUserAction()
    {
        int shifterChange = shifter->detectChange();
        int turnSignalDir = turnSignal->detectChange();
        Direction wheelDirection = wheel->getDirection();
        if (horn->getReading() == 0)
        {
            //tone(SPEAKER_PIN, 50, 500);
            return Action::HORN;
        }
        else if (shifterChange != -1)
        {
            Serial.println("SHIFT: " + String(shifterChange));
            return 0;
        }
        else if (wheelDirection != Direction::NONE)
        {
            Serial.println("STEER: " + String(wheelDirection));
            return wheelDirection == Direction::LEFT ? Action::STEER_LEFT : Action::STEER_RIGHT;
        }
        else if (turnSignalDir != -1)
        {
            Serial.println("TURN: " + String(turnSignalDir));
            return 4;
        }
        else
            return -1;
    }
    
    void runGame()
    {
        Serial.println("Game Running");
        State state = State::WAITING;
        int totalAttempts;
        int correctAttempts;
        unsigned long timeToAction = 10000;
        Action generatedAction;
        while (true)
        {
            if (state == State::WAITING)
            {
                digitalWrite(2,1);
                display->print("Waiting");
                Serial.println("Waiting for start");
                if (start->getReading() == 1)
                {
                    Serial.println("Started");
                    state = State::START;
                }
            }
            else if (state == State::START)
            {
                digitalWrite(2,0);
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
                        display->reset();
                        break;
                    }
                    else if (action != -1)
                    {
                        // Screen shows unsuccessful attempt
                        actionMade = true;
                        display->reset();
                        break;
                    }
                }
                if (action != -1)
                {
                    display->print("User chose: " + actions[action]);
                    Serial.print("User chose: ");
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
                display->reset();
                display->print("Score: " + String(correctAttempts) + "/" + String(totalAttempts));
                delay(5000);
                display->reset();
                state = State::WAITING;
            }
        }
    }
};
#endif
