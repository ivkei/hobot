#include"notesapp.h"

#include<chrono>

#include"glm/gtc/constants.hpp"

#include"GL/glew.h"
#include"GLFW/glfw3.h"

NotesApp::NotesApp()
: _pWindow(std::make_unique<hobot::Window>(hobot::WindowProps(1080, 1080, "notes", -1, false))){
  _pWindow->SetVSync(false);
}

NotesApp::~NotesApp(){
}

void NotesApp::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;
  const auto& renderer = _pWindow->GetRenderer();
  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;

  //FPS
  float fpsTimer = 0;
  int frames = 0;
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

    renderer.Clear({0.1f, 0.1f, 0.1f, 1.0f});
    renderer.Trig({-0.5f, -0.5f}, {0.5f, -0.5f}, {0.0f, 0.5f},
                  {1, 0, 0, 1},   {0, 1, 0,1},   {0, 0, 1, 1});

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
