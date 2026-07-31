#pragma once

#include<functional>

#include"asteroids/asteroids.h"
#include"ship/ship.h"

class Logic;

class AsteroidsManager{
private:
  Logic* _pLogic;
  std::function<hobot::Vec2(hobot::Vec2)> _wrapFunc;
  std::function<bool(hobot::Vec2)> _isWithinFieldFunc;
  float _asteroidsTimer;
public:
  AsteroidsManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc, std::function<bool(hobot::Vec2)> isWithinFieldFunc);
  //Ship collisions here
  void Manage(Asteroids& asteroids, float speedScale, float angularSpeedScale, Ship& ship);
};
