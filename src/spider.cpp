#include "spider.h"

void Spider::init(){

  // Initialise both PCA9685 boards at 50 Hz (standard hobby servo frequency)
  boardA.begin();
  boardA.setPWMFreq(50);

  boardB.begin();
  boardB.setPWMFreq(50);
  // Initialise each leg — pass its board pointer and three channel numbers
  for (int i = 0; i < 6; i++) {
      legs[i].init(
        LEG_BOARD[i],
        LEG_PINS[i][0],   // base servo channel
        LEG_PINS[i][1],   // hip  servo channel
        LEG_PINS[i][2]    // knee servo channel
      );
  }
}

void Spider::handle(){
  for (int i = 0; i < 6; i++) {
    legs[i].handle();
  }

  // While strafing, stop the whole robot as soon as any leg hits its
  // physical limit so the legs stay in sync.
  if (strafing_) {
    for (int i = 0; i < 6; i++) {
      if (legs[i].isBlocked()) {
        Serial.println("STRAFE STOPPED: a leg reached its physical limit");
        stop();
        break;
      }
    }
  }
}

void Spider::move_all_legs(int x, int y, int z){
  legs[0].moveToStraight(x, y, z);
  legs[1].moveToStraight(x, y, z);
  legs[2].moveToStraight(x, y, z);
  legs[3].moveToStraight(x, y, z);
  legs[4].moveToStraight(x, y, z);
  legs[5].moveToStraight(x, y, z);
}

void Spider::zero_all_legs(){
  legs[0].zero();
  legs[1].zero();
  legs[2].zero();
  legs[3].zero();
  legs[4].zero();
  legs[5].zero();
}

void Spider::strafe(Vec3 direction, float speed)
{
  // A zero direction means "don't move" — treat it as a stop.
  Vec3 dir = direction.normalized();
  if (dir.magnitudeSquared() < 1e-6f) {
    stop();
    return;
  }

  for(int i = 0; i < 6; i++)
  {
    // Rotate the horizontal (x, y) component into this leg's frame; the
    // vertical (z) component is common to every leg.
    Vec2 horizontal = Vec2(dir.x, dir.y).rotated(world_to_leg_space_angles[i]);
    Vec3 leg_velocity = Vec3(horizontal.x, horizontal.y, dir.z) * speed;
    legs[i].strafe(leg_velocity);
  }
  strafing_ = true;
}

void Spider::stop()
{
  for (int i = 0; i < 6; i++) {
    legs[i].stopStrafe();
  }
  strafing_ = false;
}