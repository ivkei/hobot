#include"shipmanager.h"

#include"logic/logic.h"

#include"hobot.h"

ShipManager::ShipManager(Logic* pLogic, std::function<glm::vec2(glm::vec2)> wrapFunc)
: _pLogic(pLogic), _wrapFunc(wrapFunc){
}

void ShipManager::Manage(Ship& ship, float speedScale, float angularSpeedScale){
  //Ships movement
  glm::vec2 newShipVel = ship.velocity;
  if (_pLogic->IsKeyPressed(hobot::Key::W)){
    newShipVel += ship.direction*speedScale*ship.speed;
  }
  if (_pLogic->IsKeyPressed(hobot::Key::S)){
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
  if (_pLogic->IsKeyPressed(hobot::Key::A)){
    rotationalDirection = -1;
  }
  if (_pLogic->IsKeyPressed(hobot::Key::D)){
    rotationalDirection = 1;
  }
  float theta = rotationalDirection*angularSpeedScale*ship.angularSpeed;
  glm::mat2x2 shipRotation = {
    std::cos(theta), -std::sin(theta),
    std::sin(theta), std::cos(theta)
  };
  ship.direction = shipRotation*ship.direction;

  //Ship edge handling (wrap position)
  ship.pos = _wrapFunc(ship.pos);
}
