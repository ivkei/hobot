#include"asteroidsmanager.h"

#include"logic/logic.h"

AsteroidsManager::AsteroidsManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc, std::function<bool(hobot::Vec2)> isWithinFieldFunc)
: _pLogic(pLogic), _wrapFunc(wrapFunc), _asteroidsTimer(0), _isWithinFieldFunc(isWithinFieldFunc){
}

void AsteroidsManager::Manage(Asteroids& asteroids, float speedScale, float angularSpeedScale, Ship& ship){
  //Add a few random asteroids if not enough
  if (asteroids.asteroids.size() < asteroids.asteroidCount && (_asteroidsTimer+=_pLogic->DeltaSeconds()) >= asteroids.asteroidInterval){
    _asteroidsTimer = 0;
    for (int i = 0; i < asteroids.asteroidsPerInterval; i++){
      hobot::Vec2 pos = {0, 0};
      while (_isWithinFieldFunc(pos = {(rand()%100-50), (rand()%100-50)})); //Not within the box 

      hobot::Vec2 velocity = ((rand()%100-50)/50.0f)*asteroids.maxAsteroidSpeed*hobot::Norm(hobot::Vec2{std::fmod((rand()%100-50)/50.0f, 2)-1, std::fmod((rand() % 100)/100.0f, 2)-1});

      float angularSpeed = (rand()%100-50) * asteroids.maxAsteroidAngularSpeed;

      float radius = 0;
      while ((radius = ((rand()%100+1)/100.0f)*asteroids.maxAsteroidRadius) < asteroids.minAsteroidRadius);

      asteroids.asteroids.emplace_back(Asteroid{pos, velocity, angularSpeed, radius, rand()%10+3, ((float)(rand()%100)/100.0f)*2*hobot::PI<float>()});
    }
  }

  //Asteroids movement and rotation
  for (auto& i : asteroids.asteroids){
    i.pos+=i.velocity*speedScale*10000.0f;
    i.rotation = std::fmod((i.rotation+i.angularSpeed*angularSpeedScale*0.005f)+2*hobot::PI<float>(), 2*hobot::PI<float>());
  }

  //Asteroid edge handling (wrap position)
  for (auto& i : asteroids.asteroids){
    i.pos = _wrapFunc(i.pos);
  }

  //Handle collisions
  for (auto& i : asteroids.asteroids){
    hobot::Vec2 originShipPos = ship.pos - i.pos;
    if (originShipPos.x*originShipPos.x + originShipPos.y*originShipPos.y < i.radius*i.radius*asteroids.radiusCollisionFactor*asteroids.radiusCollisionFactor) _pLogic->GameOver();
  }
}
