#include"app.h"

#include<chrono>

#include"renderer/renderer.h"

App::App()
: _pWindow(std::make_unique<hobot::Window>(Game::GetWindowProps())),
  _game(_pWindow->GetRenderer()){
  _pWindow->SetVSync(true);
}

App::~App(){
}

void App::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;

  //Fixed
  float fixedTimer = 0;

  //FPS
  float timer = 0;
  int frames = 0;

  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;
  while (!_pWindow->ShouldTerminate()){
    auto now = cnow();
    //From nanoseconds
    deltaSeconds = (now - lastFrame).count()*1e-9;
    lastFrame = now;

    //FPS
    frames++;
    if ((timer+=deltaSeconds)>=1){
      HT_LOG_INFO("FPS: ", frames);
      frames = 0;
      timer = 0;
    }

    //This while approach is better than deltaSeconds to correct in fixed
    //Due to more consistency
    //Suppose previous frame took ages to render yet OnFixedUpdate is called properly,
    //then its going to correct for no cause, the rendering and FixedUpdate are uncoupled in this case.
    //This, however, fixes that as it will only be called at fixed intevals, and if frames lag behind, this will make up
    fixedTimer += deltaSeconds;
    while (fixedTimer >= _game.FixedInterval()){
      _game.OnFixedUpdate();
      fixedTimer-=_game.FixedInterval();
    }

    _game.OnUpdate(deltaSeconds);

    _pWindow->PollEvents();
    if (Renderer::ToSwapBuffers()) _pWindow->SwapBuffers();
  }
}
