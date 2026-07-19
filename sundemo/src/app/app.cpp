#include"app.h"

#include<chrono>

App::App()
: _pWindow(std::make_unique<hobot::Window>(hobot::WindowProps(960, 540, "notes", -1, true))){
  _pWindow->SetVSync(true);
}

App::~App(){
}

void App::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;

  const auto& renderer = _pWindow->GetRenderer();

  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;

  //Move the stars
  float move = 0.0f;
  glm::vec2 vel = {1.0f, 0.0f};

  //FPS
  float timer = 0;
  int frames = 0;
  while (!_pWindow->ShouldTerminate()){
    auto now = cnow();
    //From nanoseconds
    deltaSeconds = (now - lastFrame).count()*1e-9;
    lastFrame = cnow();

    //FPS
    frames++;
    if ((timer+=deltaSeconds)>=1){
      HT_LOG_INFO("FPS: ", frames);
      frames = 0;
      timer = 0;
    }

    renderer.Clear({0.1f, 0.1f, 0.1f, 1.0f});

    move+=deltaSeconds;
    srand(0);
    for (int i = 0; i < 300; i++){
      auto pos = glm::vec2((rand() % 100000), (rand() % 100000)) + 50*move*vel*(float)(rand() % 100 + 1);
      pos.x = ((int)pos.x % 100000)/50000.0f-1.0f;
      pos.y = ((int)pos.y % 100000)/50000.0f-1.0f;
      renderer.Circle(pos, 0.005f, (rand() % 7) + 4, {1,1,1,1});
    }
    renderer.Circle({0, 0}, 0.3f, 75, {1.0f, 0.8f, 0.5f, 1.0f}, {0.9f, 0.4f, 0.1f, 1.0f}); //Sun

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
