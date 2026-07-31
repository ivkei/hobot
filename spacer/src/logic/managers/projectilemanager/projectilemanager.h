#pragma once

#include<functional>

#include"asteroids/asteroids.h"
#include"ship/ship.h"
#include"projectiles/projectiles.h"

class Logic;

class ProjectileManager{
private:
  Logic* _pLogic;
  std::function<hobot::Vec2(hobot::Vec2)> _wrapFunc;
  std::function<hobot::Vec2(hobot::Vec2)> _fuzzPos;
  float _reloadTimer;
public:
  ProjectileManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc, std::function<hobot::Vec2(hobot::Vec2)> fuzzPos, Projectiles& projectiles);
  void Manage(Ship& ship, Asteroids& asteroids, Projectiles& projectiles, float speedScale);
};
