#pragma once

#include<functional>

#include"glm/glm.hpp"

#include"asteroids/asteroids.h"
#include"ship/ship.h"
#include"projectiles/projectiles.h"

class Logic;

class ProjectileManager{
private:
  Logic* _pLogic;
  std::function<glm::vec2(glm::vec2)> _wrapFunc;
  std::function<glm::vec2(glm::vec2)> _fuzzPos;
  float _reloadTimer;
public:
  ProjectileManager(Logic* pLogic, std::function<glm::vec2(glm::vec2)> wrapFunc, std::function<glm::vec2(glm::vec2)> fuzzPos, Projectiles& projectiles);
  void Manage(Ship& ship, Asteroids& asteroids, Projectiles& projectiles, float speedScale);
};
