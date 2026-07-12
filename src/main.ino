#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "spider.h"
#include <SerialCommander.h>

Spider spider;
SerialCommander commander;

void onStrafe(CommandArgs& args) {
  Vec3 dir;
  dir.x = args.getInt(0);
  dir.y = args.getInt(1);
  dir.z = args.getInt(2, 0);       // vertical component, defaults to 0
  int speed = args.getInt(3, 50);  // defaults to 50 if omitted

  Serial.print("STRAFING IN DIRECTION: ");
  Serial.print(dir.x);
  Serial.print(", ");
  Serial.print(dir.y);
  Serial.print(", ");
  Serial.print(dir.z);
  Serial.print("  AT SPEED: ");
  Serial.println(speed);

  spider.strafe(dir, speed);
}
void onStop(CommandArgs& args) {
  Serial.println("STOPPING");
  spider.stop();
}
void onZero(CommandArgs& args) {
  Serial.println("ZEROING ALL LEGS");
  spider.stop();  // cancel any in-progress strafe first
  spider.zero_all_legs();
}
void onUnknown(CommandArgs& args) {
  Serial.print("Unrecognized command: ");
  Serial.println(args.raw());
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("SERIAL INITIALISED");
  Wire.begin();
  spider.init();

  commander.registerCommand("STRAFE", onStrafe);  // e.g. STRAFE:10,20,0,50  (dir x,y,z + speed)
  commander.registerCommand("STOP", onStop);      // halts an in-progress strafe
  commander.registerCommand("ZERO", onZero);      // return all legs to their neutral/home pose
  commander.setUnknownCommandCallback(onUnknown);
}

void loop() {
  spider.handle();
  if (Serial.available() > 0) {
    commander.handle();
  }
}