#pragma once

#include"hobot.h"
#include"field/field.h"

class Renderer{
private:
  static float _move;
  static bool _toSwapBuffers;
public:
  static void Render(const hobot::Renderer& renderer, Field& field, float deltaSeconds);
  static bool ToSwapBuffers() {if (_toSwapBuffers) return true; _toSwapBuffers = false; return false;}
};
