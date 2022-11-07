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
    DONE,
    LOW_BATTERY
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
        if (r == 0)
        {
            tone(SPEAKER_PIN, 400, 300);
        }
        else if (r == 2)
        {
            tone(SPEAKER_PIN, 600, 300);
        }
        else if (r == 3)
        {
            tone(SPEAKER_PIN, 250, 300);
        }
        else if (r == 4)
        {
            tone(SPEAKER_PIN, 50, 300);
        }
        else
        {
            tone(SPEAKER_PIN, 150, 300);
        }
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
        randomSeed(analogRead(0));
        // analogReference(EXTERNAL);
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
            tone(SPEAKER_PIN, 50, 500);
            return Action::HORN;
        }
        else if (shifterChange != -1)
        {
            return 0;
        }
        else if (wheelDirection != Direction::NONE)
        {
            return wheelDirection == Direction::LEFT ? Action::STEER_LEFT : Action::STEER_RIGHT;
        }
        else if (turnSignalDir != -1)
        {
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
        // unsigned long timeToAction = 10000;
        Action generatedAction;
        display->reset();
        bool startScreen = false;
        while (true)
        {
            int batteryLevel = analogRead(BATTERY_PIN);
            Serial.println("Battery Level: " + String(batteryLevel));
            if (state == State::WAITING)
            {
                digitalWrite(2, 1);
                if(!startScreen){
                  display->image("ss.bmp");
                  startScreen = true;
                }
                
                //display->print("Hit start button");
                Serial.println("Waiting for start");
                if (start->getReading() == 1)
                {
                    Serial.println("Started");
                    state = State::START;
                }
            }
            else if (state == State::START)
            {
                digitalWrite(2, 0);
                display->reset();
                display->print("Starting");
                totalAttempts = 0;
                correctAttempts = 0;
                state = State::GENERATE_ACTION;
            }
            else if (state == State::GENERATE_ACTION)
            {
                delay(2000);
                generatedAction = generateAction();
                state = State::USER_INPUT;
                display->print("Generated Action: " + actions[generatedAction]);
                Serial.print("Generated Action: ");
                Serial.println(actions[generatedAction]);
            }
            else if (state == State::USER_INPUT)
            {
                bool actionMade = false;
                unsigned long startingTime = millis();
                Action action = -1;
                float timeToAction = 1000 * (7 - .7 * log(correctAttempts + 1));
                while (millis() < startingTime + timeToAction)
                {
                    action = getUserAction();
                    if (action == generatedAction)
                    {
                        actionMade = true;
                        display->reset();
                        display->print("Correct action!");
                        correctAttempts++;
                        break;
                    }
                    else if (action != -1)
                    {
                        display->reset();
                        display->print("Wrong action!");
                        actionMade = true;
                        break;
                    }
                }

                if (action == -1)
                {
                    display->reset();
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
                startScreen = false;
            }
            else if (state == LOW_BATTERY)
            {
                display->print("LOW BATTERY: " + String(batteryLevel));
            }
        }
    }
};
#endif
