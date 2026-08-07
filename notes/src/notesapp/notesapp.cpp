#include"notesapp.h"

#include<chrono>

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

    srand(0);
    const float SQ_WIDTH = 256.0f;
    const float CL_WIDTH = 2.0f/SQ_WIDTH;
    for (float i = -SQ_WIDTH; i < SQ_WIDTH/2.0f; i++){
      for (float j = -SQ_WIDTH; j < SQ_WIDTH/2.0f; j++){
        renderer.Quad({2.0f*i/SQ_WIDTH+CL_WIDTH/2.0f, 2.0f*j/SQ_WIDTH+CL_WIDTH/2}, {CL_WIDTH, CL_WIDTH}, {rand() % 100 / 120.0f, 0, rand() % 100 / 120.0f, 1});
      }
    }
    //TODO: why so little FPS?

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
