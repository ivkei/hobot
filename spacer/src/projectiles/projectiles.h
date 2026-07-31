#pragma once

#include"hobot.h"

#include<vector>

struct Projectile{
  hobot::Vec2 pos;
  hobot::Vec2 dir;
  hobot::Vec2 velocity;
  float lifetime;
};

struct Projectiles{
  std::vector<Projectile> projectiles;
  int maxProjectileCount;
  float projectileSpeed;
  float maxLifetime;
  float radius;
  hobot::Vec4 color;
  int edges;
  float reloadTime;
  int shotDown;
  float knockback;
};
