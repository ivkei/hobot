#include"game.h"

#include"hobot.h"

#include"renderer/renderer.h"
#include"logic/logic.h"

hobot::WindowProps Game::GetWindowProps(){
  return {1080, 1080, "gsim", 4.4, false};
}

float Game::FixedInterval(){
  return 1.0f/165.0f;
  //return 0.1f;
}

template<hobot::Vec2 min, hobot::Vec2 max>
hobot::Vec2 Wrap(hobot::Vec2 v){
  if (v.x >= max.x) v.x = min.x;
  else if (v.y >= max.y) v.y = min.y;

  else if (v.x <= min.x) v.x = max.x;
  else if (v.y <= min.y) v.y = max.y;

  return v;
}

hobot::Vec2 DontWrap(hobot::Vec2 v) {return v;}

Game::Game(const hobot::Renderer& renderer)
: _renderer(renderer), _logic(DontWrap){
  //2 test objects
  //_objects.emplace_back(Object{{ 1, 0}, {0,  0.2}, 1, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{-1, 0}, {0, -0.2}, 1, 0.01f, {0, 1, 0, 1}});

  //Test different masses
  //_objects.emplace_back(Object{{1, 0}, {0, 0}, 2, 0.01f, {0.9, 0.4f, 0.1f, 1}});
  //_objects.emplace_back(Object{{-1, 0}, {0, -0.2}, 1, 0.01f, {0.15f, 0.2f, 0.7f, 1}});

  //3-body problem solution (too easy)
  //hobot::Vec2 pos1 = hobot::Vec2{1,  0};
  //hobot::Vec2 pos2 = hobot::Vec2{-1.0f/2.0f, std::sqrt(3)/2.0f};
  //hobot::Vec2 pos3 = hobot::Vec2{-1.0f/2.0f, -std::sqrt(3)/2.0f};
  //_objects.emplace_back(Object{{-pos1.y, pos1.x}, pos1/5, 1, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{-pos2.y, pos2.x}, pos2/5, 1, 0.01f, {0, 1, 0, 1}});
  //_objects.emplace_back(Object{{-pos3.y, pos3.x}, pos3/5, 1, 0.01f, {0, 0, 1, 1}});

  //Chaos gun 1
  //hobot::Vec2 pos1 = hobot::Vec2{1,  0};
  //hobot::Vec2 pos2 = hobot::Vec2{-1.0f/2.0f, std::sqrt(3)/2.0f};
  //hobot::Vec2 pos3 = hobot::Vec2{-1.0f/2.0f, -std::sqrt(3)/2.0f};
  //_objects.emplace_back(Object{{0, 0}, pos1/5+hobot::Vec2(0.1f), 1, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos2/5+hobot::Vec2{-0.1f, 0.2f}, 1, 0.01f, {0, 1, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos3/5, 1, 0.01f, {0, 0, 1, 1}});

  //Chaos gun 2
  //hobot::Vec2 pos1 = hobot::Vec2{1,  0};
  //hobot::Vec2 pos2 = hobot::Vec2{-1.0f/2.0f, std::sqrt(3)/2.0f};
  //hobot::Vec2 pos3 = hobot::Vec2{-1.0f/2.0f, -std::sqrt(3)/2.0f};
  //_objects.emplace_back(Object{{0, 0}, pos1/5+hobot::Vec2(0.1f, 0.5f), 1, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos2/5+hobot::Vec2{-0.1f, 0.2f}, 1, 0.01f, {0, 1, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos3/5, 1, 0.01f, {0, 0, 1, 1}});

  //Chaos 3
  //_logic.SetGravity(0.1f); //So that fits on the screen better
  //hobot::Vec2 pos1 = hobot::Vec2{1,  0};
  //hobot::Vec2 pos2 = hobot::Vec2{-1.0f/2.0f, std::sqrt(3)/2.0f};
  //hobot::Vec2 pos3 = hobot::Vec2{-1.0f/2.0f, -std::sqrt(3)/2.0f};
  //_objects.emplace_back(Object{{0, 0}, pos1/5+hobot::Vec2(0.1f, 0.5f), 3, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos2/5+hobot::Vec2{-0.1f, 0.2f}, 4, 0.01f, {0, 1, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos3/5, 5, 0.01f, {0, 0, 1, 1}});

  //_logic.SetGravity(0.01f);
  //hobot::Vec2 pos1 = hobot::Vec2{0, 0.4f}/10;
  //hobot::Vec2 pos2 = hobot::Vec2{0.3f, 0}/10;
  //hobot::Vec2 pos3 = hobot::Vec2{0, 0};
  //_objects.emplace_back(Object{{0, 0}, pos1, 3, 0.01f, {1, 0, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos2, 4, 0.01f, {0, 1, 0, 1}});
  //_objects.emplace_back(Object{{0, 0}, pos3, 5, 0.01f, {0, 0, 1, 1}});

  //Earth and sun (not realistic at all)
  //_logic.SetGravity(0.01f);
  //_objects.emplace_back(Object{{0, 0}, {0, 0}, 250, 0.1f, {0.9, 0.4f, 0.1f, 1}});
  //_objects.emplace_back(Object{{3, 0}, {0, -0.5}, 1, 0.01f, {0.15f, 0.2f, 0.7f, 1}});

  //Lots
  _logic.SetGravity(0.01f);
  int D = 12;
  for (int i = -D/2; i <= D/2; i++){
    for (int j = -D/2; j <= D/2; j++){
      hobot::Vec2 pos = {2.0f*i/(float)D, 2.0f*j/(float)D};
      hobot::Vec4 color = {pos.x, pos.y, 1, 1};
      _objects.emplace_back(Object{{0, 0}, pos, 1, 0.01f, color});
    }
  }
}

void Game::OnUpdate(float deltaSeconds){
}

void Game::OnFixedUpdate(){
  //No deltaSeconds because its fixed
  _logic.Execute(_objects, FixedInterval());
  Renderer::Render(_renderer, _objects, FixedInterval(), 0.5f);
}

Game::~Game(){
}
