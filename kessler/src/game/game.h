#pragma once

#include"hobot.h"

#include"field/field.h"
#include"logic/logic.h"
#include"ship/ship.h"
#include"asteroids/asteroids.h"
#include"projectiles/projectiles.h"

class Game{
private:
  const hobot::Renderer& _renderer;
  Field _field;
  Logic _logic;
  Ship _ship;
  Asteroids _asteroids;
  Projectiles _projectiles;
  float _fixedInterval;
public:
  static hobot::WindowProps GetWindowProps();

  void OnUpdate(float deltaSeconds);
  void OnFixedUpdate(float deltaSeconds);
  float GetFixedInterval() const {return _fixedInterval;};

  void Reset();

  Game(const hobot::Renderer& renderer, hobot::Window& window);
  ~Game();
};
