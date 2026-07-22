#pragma once

#include"hobot.h"

#include"field/field.h"
#include"logic/logic.h"

class Game{
private:
  const hobot::Renderer& _renderer;
  Field _field;
  Logic _logic;
  float _fixedInterval;
public:
  static hobot::WindowProps GetWindowProps();

  void OnUpdate(float deltaSeconds);
  void OnFixedUpdate(float deltaSeconds);
  float GetFixedInterval() const {return _fixedInterval;};

  Game(const hobot::Renderer& renderer, hobot::Window& window);
  ~Game();
};
