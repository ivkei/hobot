#include"game.h"

#include"render/render.h"

hobot::WindowProps Game::GetWindowProps(){
  return {1080, 1080, "spacer", -1, false};
}

static Field GetInitField(){
  return {{0.9f, 0.1f}, {0.1f, 0.1f, 0.1f, 1.0f}};
}
static Ship GetInitShip(){
  return {{0, 0}, {0, 1}, 1, {0.94f, 0.44f, 0.45f, 1}, {0, 0}, 1, 1};
}
static Asteroids GetInitAsteroids(){
  return {{}, {0.5f, 0.5f, 0.5f, 1.0f}, 100, 1, 0.2f, 0.01f, 2, 4, 2, 0.95f, 1, 10, 0.8f, 3, 1.0f/3.0f};
}
static Projectiles GetInitProjectiles(){
  return {{}, 1, 1, 7.5, 0.01f, {0.96f, 0.79f, 0.48f, 1.0f}, 12, 1.0f, 0, 1};
}

Game::Game(const hobot::Renderer& renderer, hobot::Window& window)
: _renderer(renderer),
  _field(GetInitField()),
  _ship(GetInitShip()),
  _asteroids(GetInitAsteroids()),
  _projectiles(GetInitProjectiles()),
  _logic(window, _ship, _asteroids, _projectiles, this),
  _fixedInterval(_logic.FixedInterval()){
}

void Game::OnUpdate(float deltaSeconds){
  Renderer::Render(_renderer, _field, _asteroids, _ship, _projectiles, deltaSeconds);
  _logic.Execute(_field, _asteroids, _ship, _projectiles, deltaSeconds);
}

void Game::OnFixedUpdate(float deltaSeconds){
  _logic.FixedExecute(_field, _asteroids, _ship, _projectiles, deltaSeconds);
}

void Game::Reset(){
  _ship = GetInitShip();
  _asteroids = GetInitAsteroids();
  _field = GetInitField();
  _projectiles = GetInitProjectiles();
}

Game::~Game(){
}
