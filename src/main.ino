#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "spider.h"
#include <SerialCommander.h>

Spider spider;
SerialCommander commander;

void onStrafe(CommandArgs& args) {
  Vec2 vec;
  vec.x = args.getInt(0);
  vec.y = args.getInt(1);
  int speed = args.getInt(2, 50); // defaults to 50 if 3rd arg omitted

  Serial.print("STRAFING IN DIRECTION: ");
  Serial.print(vec.x);
  Serial.print(", ");
  Serial.print(vec.y);
  Serial.print("  AT SPEED: ");
  Serial.println(speed);

  spider.strafe(vec, speed);
}
void onUnknown(CommandArgs& args) {
  Serial.print("Unrecognized command: ");
  Serial.println(args.raw());
}


void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } 
    Serial.println("SERIAL INITIALISED");
    Wire.begin();
    spider.init();

    commander.registerCommand("STRAFE", onStrafe);  // e.g. STRAFE:10,20,1
    commander.setUnknownCommandCallback(onUnknown);

}

void loop() {
    spider.handle();
    if(Serial.available()>0){
        commander.handle();
    }
}