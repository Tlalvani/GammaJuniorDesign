#ifndef STEERINGWHEEL_H
#define STEERINGWHEEL_H

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

enum Direction
{
    LEFT,
    RIGHT,
    NONE
};

class SteeringWheel
{
private:
    // class default I2C address is 0x68
    // specific I2C addresses may be passed as a parameter here
    // AD0 low = 0x68 (default for InvenSense evaluation board)
    // AD0 high = 0x69
    MPU6050 accelgyro;
    // MPU6050 accelgyro(0x69); // <-- use for AD0 high

    int16_t ax, ay, az;

public:
    SteeringWheel()
    {
        Wire.begin();
        Wire.setClock(400000);
        Serial.println("Init gyro");
        accelgyro.initialize();
        // verify connection
        Serial.println("Testing device connections...");
        Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    }
    Direction getDirection()
    {
        // TODO if statement based on accel

        return Direction::NONE;
    }
};

#endif
