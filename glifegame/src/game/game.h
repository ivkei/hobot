#pragma once

#include"hobot.h"

class Game{
private:
  const hobot::Renderer& _renderer;
  unsigned int _backSlot;
  bool _toSwapBuffers;
public:
  static hobot::WindowProps GetWindowProps();

  void OnUpdate(float deltaSeconds);

  void OnFixedUpdate(float deltaSeconds);
  float FixedInterval();

  bool ToSwapBuffers() { if (_toSwapBuffers) { _toSwapBuffers = false; return true;} return false; }

  Game(const hobot::Renderer& renderer);
  ~Game();
};
