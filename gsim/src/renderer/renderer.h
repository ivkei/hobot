#pragma once

#include"object/object.h"

struct Renderer{
private:
  static bool _swapBuffers;
  static float _move;
public:
  //Correction is responsible for handling different framerates, pass deltaSeconds or fixed values depending on when is called
  static void Render(const hobot::Renderer& renderer, const std::vector<Object>& objects, float correction, float zoom);
  static bool ToSwapBuffers() {if (_swapBuffers) {_swapBuffers = false; return true;} return false;}
};
