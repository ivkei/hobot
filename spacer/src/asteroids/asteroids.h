#pragma once

#include"hobot.h"

#include<vector>

struct Asteroid{
  hobot::Vec2 pos;
  hobot::Vec2 velocity;
  float angularSpeed;
  float radius;
  int edges;
  float rotation;
  int breakCounter;
};

struct Asteroids{
  std::vector<Asteroid> asteroids;
  hobot::Vec4 asteroidColor;
  int asteroidCount;
  float maxAsteroidSpeed;
  float maxAsteroidRadius;
  float minAsteroidRadius;
  float asteroidInterval;
  int asteroidsPerInterval;
  float maxAsteroidAngularSpeed;
  float radiusCollisionFactor; //Basically, how much radius is considered for collisions
  int breakIntoMinPieces;
  int breakIntoMaxPieces;
  float angularSpeedFactorAfterDeststruction;
  int maxBreakCapacity;
  float breakCapacityStopChance;
  float projectileKnockback;
};
