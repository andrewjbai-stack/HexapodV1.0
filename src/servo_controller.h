#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>

class servoController {
private:
    Adafruit_PWMServoDriver* _pwm     = nullptr;
    int                      _channel = -1;

    // MG996R pulse-width range (microseconds)
    static constexpr float SERVO_MIN_US   = 500.0f;
    static constexpr float SERVO_MAX_US   = 2500.0f;
    // PCA9685 operating at 50 Hz → 20 ms period, 12-bit (4096 counts)
    static constexpr float PWM_PERIOD_US  = 20000.0f;
    static constexpr int   PWM_RESOLUTION = 4096;

    // Physical angle limits for this servo slot
    int minAngle_ = 0;
    int maxAngle_ = 180;

    // Motion state
    float velocity_        = 0.0f;   // deg/s
    float currentPosition_ = 0.0f;   // degrees
    float targetPosition_  = 0.0f;   // degrees
    unsigned long lastTick = 0;

    // Convert an angle in [0, 180] → PCA9685 12-bit count
    uint16_t angleToPulse(float angle) const {
        float us = SERVO_MIN_US + (angle / 180.0f) * (SERVO_MAX_US - SERVO_MIN_US);
        return (uint16_t)((us / PWM_PERIOD_US) * PWM_RESOLUTION);
    }

public:
    // pwm     — pointer to the Adafruit_PWMServoDriver this servo lives on
    // channel — PCA9685 output channel (0-15)
    void init(Adafruit_PWMServoDriver* pwm, int channel) {
        _pwm     = pwm;
        _channel = channel;
        lastTick = millis();
    }

    void setAngleLimits(int minAngle, int maxAngle) {
        minAngle_ = minAngle;
        maxAngle_ = maxAngle;
    }

    void moveTo(float targetPosition, float velocity);
    void handle();

    float getPosition() const { return targetPosition_; }

};

#endif // SERVO_CONTROLLER_H
