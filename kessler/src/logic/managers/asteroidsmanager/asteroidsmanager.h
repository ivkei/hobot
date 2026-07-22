#pragma once

#include"glm/glm.hpp"
#include<functional>

#include"asteroids/asteroids.h"
#include"ship/ship.h"

class Logic;

class AsteroidsManager{
private:
  Logic* _pLogic;
  std::function<glm::vec2(glm::vec2)> _wrapFunc;
  std::function<bool(glm::vec2)> _isWithinFieldFunc;
  float _asteroidsTimer;
public:
  AsteroidsManager(Logic* pLogic, std::function<glm::vec2(glm::vec2)> wrapFunc, std::function<bool(glm::vec2)> isWithinFieldFunc);
  //Ship collisions here
  void Manage(Asteroids& asteroids, float speedScale, float angularSpeedScale, Ship& ship);
};
