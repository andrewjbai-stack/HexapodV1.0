#include "servo_controller.h"

void servoController::moveTo(float targetPosition, float velocity) {
  targetPosition_ = constrain(targetPosition, (float)minAngle_, (float)maxAngle_);
  velocity_ = velocity;
  lastTick = millis();
}

void servoController::handle() {
  if (_pwm == nullptr || _channel < 0) return;

  unsigned long now = millis();
  float dt = (float)(now - lastTick);  // ms since last tick
  lastTick = now;

  float dist = targetPosition_ - currentPosition_;
  if (fabsf(dist) < 0.1f) return;  // already there

  int direction = (dist > 0) ? 1 : -1;

  // Step toward target
  currentPosition_ += velocity_ * (dt / 1000.0f) * direction;

  // Clamp to [current side, target] so we never overshoot
  if (direction == 1)
    currentPosition_ = constrain(currentPosition_, 0.0f, targetPosition_);
  else
    currentPosition_ = constrain(currentPosition_, targetPosition_, 180.0f);

  // Hard physical limits always win
  currentPosition_ = constrain(currentPosition_, (float)minAngle_, (float)maxAngle_);

  _pwm->setPWM(_channel, 0, angleToPulse(currentPosition_));
}
