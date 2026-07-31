#pragma once

#include"hobot.h"

#include"object/object.h"
#include"logic/logic.h"

class Game{
private:
  const hobot::Renderer& _renderer;
  std::vector<Object> _objects;
  Logic _logic;
public:
  static hobot::WindowProps GetWindowProps();

  void OnUpdate(float deltaSeconds);

  void OnFixedUpdate();
  float FixedInterval();

  Game(const hobot::Renderer& renderer);
  ~Game();
};
