#include"renderer.h"

#include"hobot.h"

float Renderer::_move = 0;
bool Renderer::_swapBuffers = false;

void Renderer::Render(const hobot::Renderer& renderer, const std::vector<Object>& objects, float correction, float zoom){
  _move += correction;

  renderer.Clear();

  //Background
  hobot::Vec2 backgroundVel = {1, 0};

  srand(0);
  for (int i = 0; i < 300; i++){
    auto pos = hobot::Vec2((rand() % 100000), (rand() % 100000)) + 50*_move*backgroundVel*(float)(rand() % 100 + 1);
    pos.x = ((int)pos.x % 100000)/50000.0f-1.0f;
    pos.y = ((int)pos.y % 100000)/50000.0f-1.0f;
    renderer.Reg(pos, 0.005f, (rand() % 7) + 4, {1,1,1,1});
  }

  //Objects
  for (auto&& object : objects){
    renderer.Reg(object.pos*zoom, object.radius*zoom, 25, object.color);
  }

  renderer.Render();
  _swapBuffers = true;
}
