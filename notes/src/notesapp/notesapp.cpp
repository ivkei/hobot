#include"notesapp.h"

#include<chrono>

NotesApp::NotesApp()
: _pWindow(std::make_unique<hobot::Window>(hobot::WindowProps(960, 540, "notes", -1, true))){
  _pWindow->SetVSync(false);
  _pWindow->GetRenderer().VertShader();
  _pWindow->GetRenderer().FragShader();
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

    glm::vec2 pos1 = {0.2f, 0.5f};
    glm::vec2 pos2 = {0.025f, 0.0f};
    glm::vec2 pos3 = {-0.2f, 0.0f};
    glm::vec2 pos4 = {0.3f, -0.1f};
    renderer.Quad(pos1, pos2, pos3, pos4, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f});
    renderer.Circle((pos1+pos2+pos3+pos4)/4.0f, 0.01f, 30, {0.8f, 0.8f, 0.8f, 1.0f});

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
