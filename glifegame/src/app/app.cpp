#include"app.h"

#include<chrono>

App::App()
: _pWindow(std::make_unique<hobot::Window>(Game::GetWindowProps())),
  _game(_pWindow->GetRenderer()){
  _pWindow->SetVSync(false);
}

App::~App(){
}

void App::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;

  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;

  //Fixed
  float fixedTimer = 0;

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

    if ((fixedTimer+=deltaSeconds) >= _game.FixedInterval()){
      fixedTimer = 0;
      _game.OnFixedUpdate(deltaSeconds);
    }

    _game.OnUpdate(deltaSeconds);

    _pWindow->PollEvents();
    if (_game.ToSwapBuffers()) _pWindow->SwapBuffers();
  }
}
