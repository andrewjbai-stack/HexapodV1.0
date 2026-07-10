#include "leg_controller.h"
#include "servo_controller.h"

//======================================
//PRIVATE METHODS
//======================================
//Calculate angles needed for a specific coordinate. Proof is left as an exercise
legController::angleInfo legController::calculateAngles(float x, float y, float z){
    legController::angleInfo result;
    result.valid = true;

    result.theta1 =  atan2(y, x);

    float d = sqrt(pow(x, 2) + pow(y, 2)) - BASE_LENGTH;

    //Angle calculations, I am not explaining any of this in here. 
    //Ill put a desmos link in the readme
    float cosTheta3 = (-pow(SHIN_LENGTH, 2) - pow(THIGH_LENGTH, 2) + pow(d, 2) + pow(z, 2)) / (2 * SHIN_LENGTH * THIGH_LENGTH);

    // Guard: if out of range, target is unreachable
    if (cosTheta3 < -1.0f || cosTheta3 > 1.0f) {
        result.valid = false;
        return result;
    }

    result.theta3 = -acos(cosTheta3);
    result.theta2 = atan2(z, d) - atan2(SHIN_LENGTH * sin(result.theta3),
                                        THIGH_LENGTH + SHIN_LENGTH * cos(result.theta3));

    //Both are converted into degrees 
    result.theta1 = (180.0/PI) * (result.theta1) + 90;
    result.theta2 = 90 - (180.0 / PI)*result.theta2; //it is subtracted from 90 because of how the servo is positioned. 
    result.theta3 = (90-(180.0 / PI)*result.theta3)-15.44;
    result.theta3 = result.theta3 - 13.3428649261; //Magic number is because leg is curved so 
                                                    //need to acount for that when placing foot.

    if (result.theta1 < 0 || result.theta1 > 180) {
        result.valid = false;
    }

    if (result.theta2 < SERVO_TWO_MIN || result.theta2 > SERVO_TWO_MAX) {
        result.valid = false;
    }

    if (result.theta3 < SERVO_THREE_MIN || result.theta3 > SERVO_THREE_MAX) {
        result.valid = false;
    }

    return result;
}

void legController::handleMovement(){
  // Time stuff
    unsigned long now = millis();
    float dt = now - lastTick;  // ms since last tick
    lastTick = now;

    // Vector from current position to target
    Vec3 dist = targetPosition_ - currentPosition_;

    // If already at goal (within tolerance on all axes)
    if (dist.magnitude() < 0.1f) {  // 0.1 units radius
        isMoving_ = false;
        //When the leg isnt moving, check if there are any other positions in the position queue.
        if(positionQueue_.isEmpty() == false){
          //This both assigns targetPosition the next position in the queue and removes it.
          positionQueue_.pop(targetPosition_);
        }
        return;
    }
    //Set is moving to true since leg isnt at target position
    isMoving_ = true;

    // Unit vector pointing toward target — replaces your int direction
    Vec3 direction = dist.normalized();

    // Step current position toward target
    currentPosition_ = currentPosition_ + direction * (velocity_ * (dt / 1000.0f));

    //Clamps on each axis, depending on direction
    //ADD MINIMUM AND MAXIMUM BOUNDS FOR EACH AXIS
    currentPosition_.x = (direction.x > 0)
        ? constrain(currentPosition_.x, -250, targetPosition_.x)
        : constrain(currentPosition_.x, targetPosition_.x, 250);

    currentPosition_.y = (direction.y > 0)
        ? constrain(currentPosition_.y, -250, targetPosition_.y)
        : constrain(currentPosition_.y, targetPosition_.y, 250);

    currentPosition_.z = (direction.z > 0)
        ? constrain(currentPosition_.z, -200, targetPosition_.z)
        : constrain(currentPosition_.z, targetPosition_.z, 200);

    this->moveTo(currentPosition_.x, currentPosition_.y, currentPosition_.z);
}

//Move to a specific spot
void legController::moveTo(float x, float y, float z){
  legController::angleInfo targetAngles = legController::calculateAngles(x, y, z);

  //If the angles are invalid skip the command
  if (!targetAngles.valid) {
    Serial.print("WARNING: Target out of reach. Target = (");
    Serial.print(targetAngles.theta1);
    Serial.print(", ");
    Serial.print(targetAngles.theta2);
    Serial.print(", ");
    Serial.print(targetAngles.theta3);
    Serial.println(")");
    return; 
  }
  //Distances are calculated in order to scale velocity based on how fast each servo should be moving
  float dist1 = abs(targetAngles.theta1 - servo1_.getPosition());
  float dist2 = abs(targetAngles.theta2 - servo2_.getPosition());
  float dist3 = abs(targetAngles.theta3 - servo3_.getPosition());

  float maxDist = max({dist1, dist2, dist3});
  if(maxDist < 0.01f) return;
  servo1_.moveTo(targetAngles.theta1, 60 * (dist1 / maxDist));
  servo2_.moveTo(targetAngles.theta2, 60 * (dist2 / maxDist));
  servo3_.moveTo(targetAngles.theta3, 60 * (dist3 / maxDist));
    Serial.println(targetAngles.theta1);
    Serial.println(targetAngles.theta2);
    Serial.println(targetAngles.theta3);
}
//======================================
//PUBLIC METHODS
//======================================
void legController::handle(){
  //Send data to servos
  servo1_.handle();
  servo2_.handle();
  servo3_.handle();

//Find the positions each tick to move the foot in a straight line
  this->handleMovement();
}

void legController::moveToStraight(float x, float y, float z, float velocity){
  positionQueue_.push({x, y, z});

  velocity_ = velocity;
}

void legController::step(float floorHeight, float x, float y){
  moveToStraight(currentPosition_.x, currentPosition_.y, floorHeight + 10);
  moveToStraight(x, y, floorHeight + 10);
  moveToStraight(x, y, floorHeight);

}

void legController::zero(){
    servo1_.moveTo(90, 50);
    servo2_.moveTo(90, 50);
    servo3_.moveTo(90, 50);

    currentPosition_ = {245.85, 0, 0};
}

