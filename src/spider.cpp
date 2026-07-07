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

void Spider::strafe(float x, float y, float speed){

}