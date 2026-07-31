#pragma once

#include"hobot.h"

struct Ship{
  //Properties
  hobot::Vec2 pos;
  hobot::Vec2 direction; //Keep normalized
  float speed;
  hobot::Vec4 color;
  hobot::Vec2 velocity;
  float maxSpeed;
  float angularSpeed;
};

