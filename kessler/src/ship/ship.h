#pragma once

#include"glm/glm.hpp"

struct Ship{
  //Properties
  glm::vec2 pos;
  glm::vec2 direction; //Keep normalized
  float speed;
  glm::vec4 color;
  glm::vec2 velocity;
  float maxSpeed;
  float angularSpeed;
};

