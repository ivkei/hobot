#pragma once

#include"field/field.h"

#include"asteroids/asteroids.h"
#include"ship/ship.h"
#include"projectiles/projectiles.h"
#include"managers/shipmanager/shipmanager.h"
#include"managers/asteroidsmanager/asteroidsmanager.h"
#include"managers/projectilemanager/projectilemanager.h"

#include<unordered_map>

#include"hobot.h"

class Game;

class Logic{
private:
  float _deltaSeconds;
  ShipManager _shipManager;
  AsteroidsManager _asteroidsManager;
  ProjectileManager _projectileManager;
  hobot::Window& _window;
  Game* _pGame;
  float _secondsElapsed;
public:
  Logic(hobot::Window& window, Ship& initShip, Asteroids& initAsteroids, Projectiles& initProjectiles, Game* pGame);
  void Execute(Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds);

  float FixedInterval();
  void FixedExecute(Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds);

  bool IsKeyPressed(hobot::Key key){ return _window.IsKeyPressed(key);};
  float DeltaSeconds() const {return _deltaSeconds;}

  void GameOver();
};
