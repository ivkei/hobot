#include"render.h"

float Renderer::_move = 0;
bool Renderer::_toSwapBuffers = false;

void Renderer::Render(const hobot::Renderer& renderer, Field& field, float deltaSeconds){
  //Background
  renderer.Clear(field.GetBackgroundColor());

  _move+=deltaSeconds;
  srand(0);
  for (int i = 0; i < 300; i++){
    auto pos = glm::vec2((rand() % 100000), (rand() % 100000)) + 50*_move*field.GetStarVelocity()*(float)(rand() % 100 + 1);
    pos.x = ((int)pos.x % 100000)/50000.0f-1.0f;
    pos.y = ((int)pos.y % 100000)/50000.0f-1.0f;
    renderer.Circle(pos, 0.005f, (rand() % 7) + 4, {1,1,1,1});
  }
  renderer.Circle({0, 0}, 0.3f, 75, {1.0f, 0.8f, 0.5f, 1.0f}, {0.9f, 0.4f, 0.1f, 1.0f}); //Sun

  //Asteroids
  const std::vector<Asteroid>& asteroids = field.Asteroids();

  for (auto& asteroid : asteroids){
    renderer.Circle(asteroid.pos, asteroid.radius, asteroid.edges, field.GetAsteroidColor(), asteroid.rotation);
  }

  //Ship
  Ship ship = field.Ship();

  glm::vec2 pos1 = ship.pos+(ship.direction*0.02f);
  glm::vec2 pos2 = ship.pos+(glm::vec2{-ship.direction.y, ship.direction.x}*0.03f+((-ship.direction)*0.025f)); //Perpendiculars (rotation)
  glm::vec2 pos3 = ship.pos+(glm::vec2{ship.direction.y, -ship.direction.x}*0.03f+((-ship.direction)*0.025f));
  glm::vec2 pos4 = ship.pos+(-ship.direction*0.01f);

  renderer.Quad(pos2,pos1,pos4,pos3,ship.color,ship.color,ship.color,ship.color);


  renderer.Render();
  _toSwapBuffers = true;
}
