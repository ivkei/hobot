#include"game.h"

#include"render/render.h"

hobot::WindowProps Game::GetWindowProps(){
  return {1280, 720, "asteroids", -1, true};
}

//Field(glm::vec2 starVelocity, glm::vec2 initShipPos, glm::vec2 initShipDirection, float initShipSpeed,
//    glm::vec4 backgroundColor, glm::vec4 shipColor, glm::vec4 asteroidColor, float maxShipSpeed, float maxShipAngularSpeed,
//    int asteroidCount, float maxAsteroidSpeed, float maxAsteroidRadius, float _asteroidInterval, int _asteroidsPerInterval, float maxAsteroidAngularSpeed);

Game::Game(const hobot::Renderer& renderer, hobot::Window& window)
: _renderer(renderer),
  _field({1, 0}, {0, 0}, {0,1}, 1, {0.1f, 0.1f, 0.1f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, 1, 1, 20, 1, 0.2f, 2, 4, 10),
  _logic(window),
  _fixedInterval(_logic.GetSecondsInterval()){
}

void Game::OnUpdate(float deltaSeconds){
  Renderer::Render(_renderer, _field, deltaSeconds);
}

void Game::OnFixedUpdate(float deltaSeconds){
  _logic.Execute(_field, deltaSeconds);
}


Game::~Game(){
}
