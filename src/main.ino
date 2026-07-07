#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "spider.h"

// ─────────────────────────────────────────────────────────────────────────────

Spider spider;
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } 
    Serial.println("SERIAL INITIALISED");
    Wire.begin();   // SDA = GPIO 21, SCL = GPIO 22 on most ESP32 boards
                    // Call Wire.begin(SDA_PIN, SCL_PIN) if yours differ
    spider.init();

}

void loop() {
    spider.handle();
    if(Serial.available()>0){
        handleInput();
    }
}

void handleInput(){
  String inputBuffer = "";
  int coords[3] = {0,0,0};
  int num = 0;

  while (Serial.available() > 0) {
    Serial.println("WHILE LOOP RUNNING");
    char c = Serial.read();
    if(c == ',')
    {
      coords[num] = inputBuffer.toInt();
      inputBuffer = "";
      num++;
    }
    else{
      inputBuffer += c;

    }
    if(num == 3){
      Serial.print("GOING TO COORDS: ");
      Serial.print(coords[0]);
      Serial.print(", ");
      Serial.print(coords[1]);
      Serial.print(", ");
      Serial.println(coords[2]);

      spider.move_all_legs(coords[0], coords[1], coords[2]);
      while (Serial.available() > 0) {
        Serial.read(); // Read and discard
      }
    }
  }
}