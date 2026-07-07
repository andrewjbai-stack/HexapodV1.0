#ifndef SPIDER_H
#define SPIDER_H

#include <Adafruit_PWMServoDriver.h>
#include "leg_controller.h"

class Spider {
public:
  void init();
  void handle();
  void move_all_legs(int x, int y, int z);
  void zero_all_legs();

  Spider(): boardA(0x40), boardB(0x41){}
private:
  // ─────────────────────────────────────────────────────────────────────────────
  // Two PCA9685 boards on the same I2C bus.
  //   Board A (0x40) — all address pins LOW (factory default, no jumpers needed)
  //   Board B (0x41) — A0 jumper soldered HIGH
  //
  // Both share SDA/SCL (ESP32 default: GPIO 21 / 22).
  // ─────────────────────────────────────────────────────────────────────────────
  Adafruit_PWMServoDriver boardA;
  Adafruit_PWMServoDriver boardB;


  // ─────────────────────────────────────────────────────────────────────────────
  // Channel map  LEG_PINS[leg_index][servo_index]
  //
  //   leg_index  0-5  (maps to leg numbers 1-6 in serial commands)
  //   servo_index: 0 = base (θ1), 1 = hip (θ2), 2 = knee (θ3)
  //
  // Legs 0-2 → boardA (channels 0-8, leaves 9-15 free for expansion)
  // Legs 3-5 → boardB (channels 0-8, leaves 9-15 free for expansion)
  //
  // Adjust channel numbers here if your wiring differs — nothing else needs
  // to change.
  // ─────────────────────────────────────────────────────────────────────────────
  const int LEG_PINS[6][3] = {
  //  { base, hip, knee }
      {  4,    5,   6  },   // Leg 1 → boardA ch 0, 1, 2
      {  9,   10,  11  },   // Leg 2 → boardA ch 3, 4, 5
      { 13,   14,  15  },   // Leg 3 → boardA ch 6, 7, 8
      {  5,    6,   7  },   // Leg 4 → boardB ch 0, 1, 2
      {  9,    10,   11  },   // Leg 5 → boardB ch 3, 4, 5
      {  13,    14,  15  },   // Leg 6 → boardB ch 6, 7, 8
  };

  // Which PCA9685 board each leg is wired to (parallel to LEG_PINS rows)
  Adafruit_PWMServoDriver* const LEG_BOARD[6] = {
      &boardA, &boardA, &boardA,   // Legs 1-3
      &boardB, &boardB, &boardB,   // Legs 4-6
  };

  // ─────────────────────────────────────────────────────────────────────────────
  // Controllers
  // ─────────────────────────────────────────────────────────────────────────────
  legController    legs[6];


};

#endif // SPIDER_H
