#pragma once

#include"hobot.h"

struct Object{
  hobot::Vec2 vel;
  hobot::Vec2 pos;
  float mass;
  float radius;
  hobot::Vec4 color;
};
