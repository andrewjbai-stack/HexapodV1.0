#ifndef SPIDER_H
#define SPIDER_H

#include <Adafruit_PWMServoDriver.h>
#include "leg_controller.h"
#include <math.h>

class Spider {
public:
  void init();
  void handle();

  // Movement functions
  void move_all_legs(int x, int y, int z);
  void zero_all_legs();
  void strafe(float x, float y, float speed);
  Spider(): boardA(0x40), boardB(0x41){}
  
private:
  Adafruit_PWMServoDriver boardA;
  Adafruit_PWMServoDriver boardB;


  const int LEG_PINS[6][3] = {
  //  { base, hip, knee }
      {  4,    5,   6  }, 
      {  9,   10,  11  }, 
      { 13,   14,  15  }, 
      {  5,    6,   7  }, 
      {  9,    10,   11  }, 
      {  13,    14,  15  }, 
  };

  // Which PCA9685 board each leg is wired to
  Adafruit_PWMServoDriver* const LEG_BOARD[6] = {
      &boardA, &boardA, &boardA,  
      &boardB, &boardB, &boardB,  
  };

  legController legs[6];


};

#endif // SPIDER_H
