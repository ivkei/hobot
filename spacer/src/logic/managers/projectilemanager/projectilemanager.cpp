#include"projectilemanager.h"

#include"logic/logic.h"

ProjectileManager::ProjectileManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc, std::function<hobot::Vec2(hobot::Vec2)> fuzzPos, Projectiles& projectiles)
: _pLogic(pLogic), _wrapFunc(wrapFunc), _fuzzPos(fuzzPos), _reloadTimer(projectiles.reloadTime){
}

void ProjectileManager::Manage(Ship& ship, Asteroids& asteroids, Projectiles& projectiles, float speedScale){
  //Shoot
  _reloadTimer+=_pLogic->DeltaSeconds();
  if (_pLogic->IsKeyPressed(hobot::Key::SPC) && projectiles.maxProjectileCount > projectiles.projectiles.size() && (_reloadTimer) >= projectiles.reloadTime){
    _reloadTimer = 0;
    projectiles.projectiles.emplace_back(ship.pos, ship.direction, ship.direction+ship.velocity*5000.0f);
  }

  //Move projectiles
  for (auto& i : projectiles.projectiles){
    i.pos += i.velocity*projectiles.projectileSpeed*speedScale*10000.0f;
  }

  //Handle lifetimes
  for (int i = 0; i < projectiles.projectiles.size(); i++){
    auto& projectile = projectiles.projectiles[i];
    if ((projectile.lifetime += _pLogic->DeltaSeconds()) >= projectiles.maxLifetime){
      projectiles.projectiles.erase(projectiles.projectiles.begin()+i);
    }
  }

  //Handle wrapping
  for (auto& i : projectiles.projectiles){
    i.pos = _wrapFunc(i.pos);
  }

  //Handle destroying asteroids
  std::vector<Asteroid> asteroidsToAdd;
  for (int i = 0; i < asteroids.asteroids.size(); i++){
    auto asteroid = asteroids.asteroids[i];
    bool broken = false;

    for (int j = 0; j < projectiles.projectiles.size() && !broken; j++){
      auto& projectile = projectiles.projectiles[j];

      hobot::Vec2 projectileShiftedPos = projectile.pos - asteroid.pos;
      if (projectileShiftedPos.x*projectileShiftedPos.x + projectileShiftedPos.y*projectileShiftedPos.y < asteroid.radius*asteroid.radius*asteroids.radiusCollisionFactor*asteroids.radiusCollisionFactor){

        //Projectile colided with asteroid
        asteroids.asteroids.erase(asteroids.asteroids.begin()+i);
        projectiles.projectiles.erase(projectiles.projectiles.begin()+j);
        projectiles.shotDown++;
        broken = true;

        //If cant break no more or by chance
        if (asteroid.breakCounter >= asteroids.maxBreakCapacity || rand()%(int)(asteroids.breakCapacityStopChance*100)>rand()%100) {
          continue;
        }

        int pieces = rand()%asteroids.breakIntoMaxPieces + asteroids.breakIntoMinPieces;
        for (int k = 0; k < pieces; k++){
          //Add asteroid pieces back
          hobot::Vec2 newPos = _fuzzPos(asteroid.pos);
          hobot::Vec2 newVelocity = hobot::Norm(hobot::Vec2{(rand()%100)/50.0f-1, (rand()%100)/50.0f-1}+projectile.velocity*projectiles.knockback+asteroid.velocity)
            *asteroids.maxAsteroidSpeed*((rand()%100+1)/100.0f)+asteroid.velocity*0.25f+projectile.velocity*0.125f/(asteroid.radius*10.0f);

          float newAngularSpeed = asteroid.angularSpeed*asteroids.angularSpeedFactorAfterDeststruction;

          float newRadius = std::fmod(((rand()%100)/100.0f)*asteroids.maxAsteroidRadius, asteroid.radius-asteroids.minAsteroidRadius)+asteroids.minAsteroidRadius;

          int newEdges = asteroid.edges;
          float newRotation = std::fmod(asteroid.rotation + ((rand()%100)/100.0f)*2*hobot::PI<float>(), 2*hobot::PI<float>());
          asteroidsToAdd.emplace_back(newPos, newVelocity, newAngularSpeed, newRadius, newEdges, newRotation, asteroid.breakCounter+1);
        }
      }
    }
  }

  asteroids.asteroids.insert(asteroids.asteroids.end(), asteroidsToAdd.begin(), asteroidsToAdd.end());
}

//hobot::Vec2 pos;
//hobot::Vec2 velocity;
//float angularSpeed;
//float radius;
//int edges;
//float rotation;
