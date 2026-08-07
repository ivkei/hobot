#include"shipmanager.h"

#include"logic/logic.h"

#include"hobot.h"

ShipManager::ShipManager(Logic* pLogic, std::function<hobot::Vec2(hobot::Vec2)> wrapFunc)
: _pLogic(pLogic), _wrapFunc(wrapFunc){
}

void ShipManager::Manage(Ship& ship, float speedScale, float angularSpeedScale){
  //Ships movement
  hobot::Vec2 newShipVel = ship.velocity;
  if (_pLogic->IsKeyPressed(hobot::Key::W)){
    newShipVel += ship.direction*speedScale*ship.speed;
    //Acceleration*dt = velocity
  }
  if (_pLogic->IsKeyPressed(hobot::Key::S)){
    newShipVel *= (1.0f-10000.0f*(speedScale*ship.speed));
  }
  //Clamp velocity
  float maxSpeedScale = 0.0005f;
  float shipVelMagnitude = hobot::Abs(newShipVel);
  if (shipVelMagnitude > ship.maxSpeed*maxSpeedScale) newShipVel = hobot::Norm(newShipVel)*ship.maxSpeed*maxSpeedScale;

  //Add ships vel
  ship.velocity=newShipVel;
  ship.pos+=ship.velocity*speedScale*50000000.0f; //NOTE: speedScale must be applied TWICE as its acceleration (Physics)
  //Velocity*dt = pos

  //Rotation
  float rotationalDirection = 0;
  if (_pLogic->IsKeyPressed(hobot::Key::A)){
    rotationalDirection = 1;
  }
  if (_pLogic->IsKeyPressed(hobot::Key::D)){
    rotationalDirection = -1;
  }
  float theta = rotationalDirection*angularSpeedScale*ship.angularSpeed;
  ship.direction = hobot::Rotate(theta, ship.direction);

  //Ship edge handling (wrap position)
  ship.pos = _wrapFunc(ship.pos);
}
