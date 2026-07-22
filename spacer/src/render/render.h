#pragma once

#include"hobot.h"
#include"field/field.h"
#include"asteroids/asteroids.h"
#include"ship/ship.h"
#include"projectiles/projectiles.h"

class Renderer{
private:
  static float _move;
  static bool _toSwapBuffers;
public:
  static void Render(const hobot::Renderer& renderer, Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds);
  static bool ToSwapBuffers() {if (_toSwapBuffers) return true; _toSwapBuffers = false; return false;}
};
