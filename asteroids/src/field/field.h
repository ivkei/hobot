#pragma once

#include"hobot.h"

#include<vector>

#include"glm/vec2.hpp"
#include"glm/vec4.hpp"

struct Asteroid{
  glm::vec2 pos;
  glm::vec2 velocity;
  float angularSpeed;
  float radius;
  int edges;
  float rotation;
};

struct Ship{
  glm::vec2 pos;
  glm::vec2 direction; //Keep normalized
  float speed;
  glm::vec4 color;
  glm::vec2 velocity;
  float maxSpeed;
  float angularSpeed;
};

class Field{
private:
  glm::vec2 _starVelocity;
  std::vector<Asteroid> _asteroids;
  Ship _ship;
  glm::vec4 _backgroundColor;
  glm::vec4 _asteroidColor;
  int _asteroidCount;
  float _maxAsteroidSpeed;
  float _maxAsteroidRadius;
  float _asteroidInterval;
  int _asteroidsPerInterval;
  float _maxAsteroidAngularSpeed;
public:
  //Cell size is between 0 and 1
  Field(glm::vec2 starVelocity, glm::vec2 initShipPos, glm::vec2 initShipDirection, float initShipSpeed,
      glm::vec4 backgroundColor, glm::vec4 shipColor, glm::vec4 asteroidColor, float maxShipSpeed, float maxShipAngularSpeed,
      int asteroidCount, float maxAsteroidSpeed, float maxAsteroidRadius, float _asteroidInterval, int _asteroidsPerInterval, float maxAsteroidAngularSpeed);
  std::vector<Asteroid>& Asteroids() {return _asteroids;}
  Ship& Ship() {return _ship;}
  glm::vec2 GetStarVelocity() const {return _starVelocity;}
  glm::vec4 GetBackgroundColor() const {return _backgroundColor;}
  glm::vec4 GetAsteroidColor() const {return _asteroidColor;}
  int GetAsteroidCount() const {return _asteroidCount;}
  int GetMaxAsteroidSpeed() const {return _maxAsteroidSpeed;}
  float GetMaxAsteroidRadius() const {return _maxAsteroidRadius;}
  float GetAsteroidInterval() const {return _asteroidInterval;};
  int GetAsteroidsPerInterval() const {return _asteroidsPerInterval;};
  int GetMaxAsteroidAngularSpeed() const {return _maxAsteroidAngularSpeed;};
};
