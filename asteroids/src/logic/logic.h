#pragma once

#include"field/field.h"

#include<unordered_map>

#include"hobot.h"

class Logic{
private:
  std::unordered_map<hobot::Key, bool> _keysPressed;
  float _asteroidTimer;
public:
  Logic(hobot::Window& window);
  //How often logic should execute
  static float GetSecondsInterval();
  void Execute(Field& field, float deltaSeconds);
};
