#pragma once

#include<vector>
#include"glm/glm.hpp"

struct Projectile{
  glm::vec2 pos;
  glm::vec2 dir;
  glm::vec2 velocity;
  float lifetime;
};

struct Projectiles{
  std::vector<Projectile> projectiles;
  int maxProjectileCount;
  float projectileSpeed;
  float maxLifetime;
  float radius;
  glm::vec4 color;
  int edges;
  float reloadTime;
  int shotDown;
};
