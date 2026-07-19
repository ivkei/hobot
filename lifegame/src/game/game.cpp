#include"game.h"

#include"render/render.h"

#include"hobot.h"

hobot::WindowProps Game::GetWindowProps(){
  return {1280, 720, "lifegame", -1, true};
}

Game::Game(const hobot::Renderer& renderer)
: _renderer(renderer),
  _field({128, 72}, {0.85f, 0.08f, 0.2f, 1.0f}, {0.9f, 0.9f}, {0.15f, 0.15f, 0.15f, 1.0f}, {0.1f, 0.1f, 0.1f, 1.0f}, 50){
}

void Game::OnUpdate(float deltaSeconds){
  _executeTimer+=deltaSeconds;
  if (_executeTimer >= _logic.GetSecondsInterval()){
    _executeTimer = 0;
    _logic.Execute(_field);

    Render(_renderer, _field);
    _toRender = true;
  }

}


Game::~Game(){
}
