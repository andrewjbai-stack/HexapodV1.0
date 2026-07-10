#ifndef LEG_CONTROLLER_H
#define LEG_CONTROLLER_H

#include "servo_controller.h"
#include "../lib/helper.h"


class legController{


public:
  //Sets pins to servos
  void init(Adafruit_PWMServoDriver* pwm, int pin1, int pin2, int pin3) {
    isMoving_ = false;
    strafing_ = false;
    blocked_  = false;

    servo3_.init(pwm, pin3);
    servo2_.init(pwm, pin2);
    servo1_.init(pwm, pin1);

    servo2_.setAngleLimits(SERVO_TWO_MIN, SERVO_TWO_MAX);
    servo3_.setAngleLimits(SERVO_THREE_MIN, SERVO_THREE_MAX);

    targetPosition_ = {120, 0, -120};
    velocity_ = 0;
    lastTick = millis();
    currentPosition_ = {120, 0, -120};

    this->moveTo(120, 0, -120);
  }

  //Calls movementHandle and children servo handle()
  void handle();

  //Moves in a straight line to the target position at the given velocity [units/sec]
  void moveToStraight(float x, float y, float z, float velocity = 50);

  //This is just a wrapper for 3 moveToStraight calls
  //It moves it straight up, over to target position, then down to floor
  void step(float floorHeight, float x, float y);

  void zero();

  //Continuously moves the foot along velocity [units/sec, leg space] every tick,
  //until stopStrafe() is called or the next position becomes unreachable.
  void strafe(Vec3 velocity);

  //Stops continuous strafing and holds the current position.
  void stopStrafe();

  //True once a strafe step was rejected because the target was unreachable
  //(i.e. the leg hit a physical limit). Reset when strafe() is called.
  bool isBlocked() { return blocked_; }

  Vec3 getPosition()  { return currentPosition_; }
private:

//CONSTANTS===========================================
  //Section lengths [in mm]
  static constexpr float SHIN_LENGTH = 114.434;
  static constexpr float THIGH_LENGTH = 87.06;
  static constexpr float BASE_LENGTH = 44.42;

  //Servo connections
  servoController servo1_;
  servoController servo2_;
  servoController servo3_;

  //Servo limits [All in degrees]
  static const int SERVO_THREE_MIN = 0;
  static const int SERVO_THREE_MAX = 180; 
  static const int SERVO_TWO_MIN = 0; 
  static const int SERVO_TWO_MAX = 180;
//STRUCTS================================================================
//Holds data representing the three angles for the three servos
//theta 1 = base, theta2 = hip, theta3 = knee.
struct angleInfo{ 
  float theta1;
  float theta2;
  float theta3;
  bool valid;
};

//VARIABLES=====================================================
  Vec3 currentPosition_;
  Vec3 targetPosition_;
  float velocity_;
  unsigned long lastTick;
  bool isMoving_;

  //Continuous strafe state
  Vec3 strafeVelocity_;   //units/sec, leg space
  bool strafing_;
  bool blocked_;

  Queue<Vec3, 8> positionQueue_;

//PRIVATE METHODS==========================

  angleInfo calculateAngles(float x, float y, float z);

  void handleMovement();

  void moveTo(float x, float y, float z);
};

#endif
