#include"logic.h"

#include"hobot.h"

#include"game/game.h"

//How often logic should execute
float Logic::FixedInterval() {return 1.0f;}
void Logic::FixedExecute(Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds){
  HT_LOG_INFO("Score: ", (int)std::round(_secondsElapsed)*10 + projectiles.shotDown*100);
}

template<hobot::Vec2 min, hobot::Vec2 max>
static hobot::Vec2 Wrap(hobot::Vec2 v){
  if (v.x > max.x) v.x = min.x;
  else if (v.x < min.x) v.x = max.x;
  if (v.y > max.y) v.y = min.y;
  else if (v.y < min.y) v.y = max.y;
  return v;
}

static hobot::Vec2 Wrap(hobot::Vec2 v, hobot::Vec2 min, hobot::Vec2 max){
  if (v.x > max.x) v.x = min.x;
  else if (v.x < min.x) v.x = max.x;
  if (v.y > max.y) v.y = min.y;
  else if (v.y < min.y) v.y = max.y;
  return v;
}

static bool IsWithin(hobot::Vec2 pos, hobot::Vec2 min, hobot::Vec2 max){
  return (pos.x > min.x && pos.y > min.y && pos.x < max.x && pos.y < max.y);
}

static hobot::Vec2 FuzzPos(hobot::Vec2 pos, hobot::Vec2 min, hobot::Vec2 max){
  hobot::Vec2 randVec = {min.x*((rand()%100)/100.0f) + max.x*((rand()%100)/100.0f), min.y*((rand()%100)/100.0f) + max.y*((rand()%100)/100.0f)};

  return pos+randVec;
}

//AsteroidsManager::AsteroidsManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc, std::function<bool(hobot::Vec2)> isWithinFieldFunc)

#define AsteroidsBounds \
{-1.0f-initAsteroids.maxAsteroidRadius, -1.0f-initAsteroids.maxAsteroidRadius},\
{1.0f+initAsteroids.maxAsteroidRadius, 1.0f+initAsteroids.maxAsteroidRadius}

Logic::Logic(hobot::Window& window, Ship& initShip, Asteroids& initAsteroids, Projectiles& initProjectiles, Game* pGame)
: _deltaSeconds(0), _shipManager(this, Wrap<{-1.05f, -1.05f}, {1.05f, 1.05f}>),
  _asteroidsManager(this, [&](hobot::Vec2 pos){return Wrap(pos, AsteroidsBounds);},
                    [&](hobot::Vec2 pos){ return IsWithin(pos, AsteroidsBounds);}), //Safe to assume address will be valid (managed by game that also manages logic)
  _pGame(pGame), _projectileManager(this, Wrap<{-1.05f, -1.05f}, {1.05f, 1.05f}>, [](hobot::Vec2 pos){return FuzzPos(pos, {-0.05f, -0.05f}, {0.05f, 0.05f});}, initProjectiles),
  _secondsElapsed(0), _window(window){
}

void Logic::Execute(Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds){
  //Scales
  float speedScale = 0.00005f*deltaSeconds;
  float angularSpeedScale = 2.0f*deltaSeconds;

  _deltaSeconds = deltaSeconds;
  _secondsElapsed+=deltaSeconds;

  //Random
  float seed = time(NULL)*deltaSeconds*1000.0f-ship.pos.x;
  if (!asteroids.asteroids.empty()) seed*=asteroids.asteroids[0].pos.x+asteroids.asteroids[0].rotation;
  srand(seed);

  //Managers
  _shipManager.Manage(ship, speedScale, angularSpeedScale);
  _asteroidsManager.Manage(asteroids, speedScale, angularSpeedScale, ship);
  _projectileManager.Manage(ship, asteroids, projectiles, speedScale);
}

void Logic::GameOver(){
  _pGame->Reset();
  _secondsElapsed = 0;
}
