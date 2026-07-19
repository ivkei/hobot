#pragma once

#include"hobot.h"

#include"field/field.h"
#include"logic/logic.h"

class Game{
private:
  const hobot::Renderer& _renderer;
  Field _field;
  Logic _logic;
  float _executeTimer;
  bool _toRender;
public:
  static hobot::WindowProps GetWindowProps();

  void OnUpdate(float deltaSeconds);
  Game(const hobot::Renderer& renderer);
  ~Game();
  bool ToRender() {if (_toRender) {_toRender = false; return true;} return false;};
};
