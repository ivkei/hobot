#include"logic.h"

#include"glm/glm.hpp"
#include"glm/gtc/constants.hpp"

#include"game/game.h"

//How often logic should execute
float Logic::FixedInterval() {return 1.0f;}
void Logic::FixedExecute(Field& field, Asteroids& asteroids, Ship& ship, Projectiles& projectiles, float deltaSeconds){
  HT_LOG_INFO("Score: ", (int)std::round(_secondsElapsed)*10 + projectiles.shotDown*100);
}

#define KeyCallback(key)\
  if (!_keysPressed.contains(hobot::Key::key)) _keysPressed.emplace(hobot::Key::key, false);\
  window.SetCallback(hobot::Key::key, [&](){\
    this->_keysPressed[hobot::Key::key] = true;\
  });\

template<glm::vec2 min, glm::vec2 max>
static glm::vec2 Wrap(glm::vec2 v){
  if (v.x > max.x) v.x = min.x;
  else if (v.x < min.x) v.x = max.x;
  if (v.y > max.y) v.y = min.y;
  else if (v.y < min.y) v.y = max.y;
  return v;
}

static glm::vec2 Wrap(glm::vec2 v, glm::vec2 min, glm::vec2 max){
  if (v.x > max.x) v.x = min.x;
  else if (v.x < min.x) v.x = max.x;
  if (v.y > max.y) v.y = min.y;
  else if (v.y < min.y) v.y = max.y;
  return v;
}

static bool IsWithin(glm::vec2 pos, glm::vec2 min, glm::vec2 max){
  return (pos.x > min.x && pos.y > min.y && pos.x < max.x && pos.y < max.y);
}

static glm::vec2 FuzzPos(glm::vec2 pos, glm::vec2 min, glm::vec2 max){
  glm::vec2 randVec = {min.x*((rand()%100)/100.0f) + max.x*((rand()%100)/100.0f), min.y*((rand()%100)/100.0f) + max.y*((rand()%100)/100.0f)};

  return pos+randVec;
}

//AsteroidsManager::AsteroidsManager(Logic* pLogic, std::function<glm::vec2(glm::vec2)> wrapFunc, std::function<bool(glm::vec2)> isWithinFieldFunc)

#define AsteroidsBounds \
{-1.0f-initAsteroids.maxAsteroidRadius, -1.0f-initAsteroids.maxAsteroidRadius},\
{1.0f+initAsteroids.maxAsteroidRadius, 1.0f+initAsteroids.maxAsteroidRadius}

Logic::Logic(hobot::Window& window, Ship& initShip, Asteroids& initAsteroids, Projectiles& initProjectiles, Game* pGame)
: _deltaSeconds(0), _shipManager(this, Wrap<{-1.05f, -1.05f}, {1.05f, 1.05f}>),
  _asteroidsManager(this, [&](glm::vec2 pos){return Wrap(pos, AsteroidsBounds);},
                    [&](glm::vec2 pos){ return IsWithin(pos, AsteroidsBounds);}), //Safe to assume address will be valid (managed by game that also manages logic)
  _pGame(pGame), _projectileManager(this, Wrap<{-1.05f, -1.05f}, {1.05f, 1.05f}>, [](glm::vec2 pos){return FuzzPos(pos, {-0.05f, -0.05f}, {0.05f, 0.05f});}, initProjectiles),
  _secondsElapsed(0){
  //Safe to assume logic will not be destroyed as game and app manage window and it
  KeyCallback(W);
  KeyCallback(A);
  KeyCallback(S);
  KeyCallback(D);
  KeyCallback(SPC);
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

  //Reset inputs
  for (auto& i : _keysPressed){
    i.second = false;
  }

}

void Logic::GameOver(){
  _pGame->Reset();
}
