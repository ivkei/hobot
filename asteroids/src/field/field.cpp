#include"field.h"

Field::Field(glm::vec2 starVelocity, glm::vec2 shipPos, glm::vec2 shipDirection, float shipSpeed, glm::vec4 backgroundColor,
    glm::vec4 shipColor, glm::vec4 asteroidColor, float maxShipSpeed, float maxShipAngularSpeed,
    int asteroidCount, float maxAsteroidSpeed, float maxAsteroidRadius, float asteroidInterval, int asteroidsPerInterval, float maxAsteroidAngularSpeed)
: _starVelocity(starVelocity),
  _ship(shipPos, shipDirection, shipSpeed, shipColor, {0, 0}, maxShipSpeed, maxShipAngularSpeed),
  _backgroundColor(backgroundColor), _asteroidColor(asteroidColor), _asteroidCount(asteroidCount),
  _maxAsteroidSpeed(maxAsteroidSpeed), _maxAsteroidRadius(maxAsteroidRadius), _asteroidsPerInterval(asteroidsPerInterval), _asteroidInterval(asteroidInterval),
  _maxAsteroidAngularSpeed(maxAsteroidAngularSpeed){
}
