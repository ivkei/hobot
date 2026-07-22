//TODO: this could use some refactoring

#include"logic.h"

#include"glm/glm.hpp"
#include"glm/gtc/constants.hpp"

//How often logic should execute
float Logic::GetSecondsInterval(){
  return 0.0f; //Every frame
}

#define KeyCallback(key)\
  if (!_keysPressed.contains(hobot::Key::key)) _keysPressed.emplace(hobot::Key::key, false);\
  window.SetCallback(hobot::Key::key, [&](){\
    this->_keysPressed[hobot::Key::key] = true;\
  });\

Logic::Logic(hobot::Window& window)
: _asteroidTimer(0){
  //Safe to assume logic will not be destroyed as game and app manage window and it
  KeyCallback(W);
  KeyCallback(A);
  KeyCallback(S);
  KeyCallback(D);
  KeyCallback(SPC);
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

static void ProcessShip(Ship& ship, Field& field, std::unordered_map<hobot::Key, bool>& keysPressed, float speedScale, float angularSpeedScale){
  //Ships movement
  glm::vec2 newShipVel = ship.velocity;
  if (keysPressed[hobot::Key::W]){
    newShipVel += ship.direction*speedScale*ship.speed;
  }
  if (keysPressed[hobot::Key::S]){
    newShipVel *= (1.0f-10000.0f*(speedScale*ship.speed));
  }
  //Clamp velocity
  float maxSpeedScale = 0.0005f;
  float shipVelMagnitude = glm::length(newShipVel);
  if (shipVelMagnitude > ship.maxSpeed*maxSpeedScale) newShipVel = glm::normalize(newShipVel)*ship.maxSpeed*maxSpeedScale;

  //Add ships vel
  ship.velocity=newShipVel;
  ship.pos+=ship.velocity;

  //Rotation
  float rotationalDirection = 0;
  if (keysPressed[hobot::Key::A]){
    rotationalDirection = -1;
  }
  if (keysPressed[hobot::Key::D]){
    rotationalDirection = 1;
  }
  float theta = rotationalDirection*angularSpeedScale*ship.angularSpeed;
  glm::mat2x2 shipRotation = {
    std::cos(theta), -std::sin(theta),
    std::sin(theta), std::cos(theta)
  };
  ship.direction = shipRotation*ship.direction;

  //Ship edge handling (wrap position)
  ship.pos = Wrap(ship.pos, {-1.05f, -1.05f}, {1.05f, 1.05f});
}

static void ProcessAsteroids(std::vector<Asteroid>& asteroids, Field& field, float& asteroidTimer, float deltaSeconds, float speedScale, float angularSpeedScale){
  //Add a few random asteroids if not enough
  if (asteroids.size() < field.GetAsteroidCount() && (asteroidTimer+=deltaSeconds) >= field.GetAsteroidInterval()){
    asteroidTimer = 0;
    for (int i = 0; i < field.GetAsteroidsPerInterval(); i++){
      glm::vec2 pos = {0, 0};
      while (IsWithin((pos = {(rand()%100-50), (rand()%100-50)}), {-1-field.GetMaxAsteroidRadius(), -1-field.GetMaxAsteroidRadius()}, {1+field.GetMaxAsteroidRadius(), 1+field.GetMaxAsteroidRadius()})); //Not within the box 

      glm::vec2 velocity = ((rand()%100-50)/50.0f)*field.GetMaxAsteroidSpeed()*glm::normalize(glm::vec2{std::fmod((rand()%100-50)/50.0f, 2)-1, std::fmod((rand() % 100)/100.0f, 2)-1});

      float angularSpeed = (rand()%100-50) * field.GetMaxAsteroidAngularSpeed();

      float radius = ((rand()%100+1)/100.0f)*field.GetMaxAsteroidRadius();

      asteroids.emplace_back(Asteroid{pos, velocity, angularSpeed, radius, rand()%10+3, ((float)(rand()%100)/100.0f)*2*glm::pi<float>()});
    }
  }

  //Asteroids movement and rotation
  for (auto& i : asteroids){
    i.pos+=i.velocity*speedScale*10000.0f;
    i.rotation = std::fmod((i.rotation+i.angularSpeed*angularSpeedScale*0.005f)+2*glm::pi<float>(), 2*glm::pi<float>());
  }

  //Asteroid edge handling (wrap position)
  for (auto& i : asteroids){
    i.pos = Wrap(i.pos, {-1-field.GetMaxAsteroidRadius(), -1-field.GetMaxAsteroidRadius()}, {1+field.GetMaxAsteroidRadius(), 1+field.GetMaxAsteroidRadius()});
  }

}

void Logic::Execute(Field& field, float deltaSeconds){
  float speedScale = 0.00005f*deltaSeconds;
  float angularSpeedScale = 2.0f*deltaSeconds;

  Ship& ship = field.Ship();
  std::vector<Asteroid>& asteroids = field.Asteroids();

  //Random
  float seed = time(NULL)*deltaSeconds*1000.0f-ship.pos.x;
  if (!asteroids.empty()) seed*=asteroids[0].pos.x+asteroids[0].rotation;
  srand(seed);

  ProcessShip(ship, field, _keysPressed, speedScale, angularSpeedScale);
  ProcessAsteroids(asteroids, field, _asteroidTimer, deltaSeconds, speedScale, angularSpeedScale);

  //Reset inputs
  for (auto& i : _keysPressed){
    i.second = false;
  }
}
