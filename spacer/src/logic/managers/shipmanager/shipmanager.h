#pragma once

#include<functional>

#include"hobot.h"

#include"asteroids/asteroids.h"
#include"ship/ship.h"

class Logic;

class ShipManager{
private:
  Logic* _pLogic;
  std::function<hobot::Vec2(hobot::Vec2)> _wrapFunc;
public:
  ShipManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc);
  void Manage(Ship& ship, float speedScale, float angularSpeedScale);
};
