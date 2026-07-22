#pragma once

#include<functional>

#include"glm/glm.hpp"

#include"asteroids/asteroids.h"
#include"ship/ship.h"

class Logic;

class ShipManager{
private:
  Logic* _pLogic;
  std::function<glm::vec2(glm::vec2)> _wrapFunc;
public:
  ShipManager(Logic* pLogic, std::function<glm::vec2(glm::vec2)> wrapFunc);
  void Manage(Ship& ship, float speedScale, float angularSpeedScale);
};
