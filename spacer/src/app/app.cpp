#include"app.h"

#include<chrono>

#include"render/render.h"

App::App()
: _pWindow(std::make_unique<hobot::Window>(Game::GetWindowProps())),
  _game(_pWindow->GetRenderer(), *_pWindow){
  _pWindow->SetVSync(false);
}

App::~App(){
}

void App::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;

  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;

  //FPS
  float fpsTimer = 0;
  int frames = 0;

  //Fixed update
  float fixedUpdateTimer = 0;
  while (!_pWindow->ShouldTerminate()){
    auto now = cnow();
    //From nanoseconds
    deltaSeconds = (now - lastFrame).count()*1e-9;
    lastFrame = cnow();

    //FPS
    frames++;
    if ((fpsTimer+=deltaSeconds)>=1){
      HT_LOG_INFO("FPS: ", frames);
      frames = 0;
      fpsTimer = 0;
    }

    if ((fixedUpdateTimer+=deltaSeconds)>=_game.GetFixedInterval()){
      fixedUpdateTimer = 0;
      _game.OnFixedUpdate(deltaSeconds);
    }

    _game.OnUpdate(deltaSeconds);

    _pWindow->PollEvents();
    if (Renderer::ToSwapBuffers()) _pWindow->SwapBuffers();
  }
}
