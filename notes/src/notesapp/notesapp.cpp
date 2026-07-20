#include"notesapp.h"

#include<chrono>

NotesApp::NotesApp()
: _pWindow(std::make_unique<hobot::Window>(hobot::WindowProps(960, 540, "notes", -1, true))){
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

  //Test raw
  std::vector<float> rawVbo = {
    -0.2f, 0.5f, 1, 1, 1, 1,
    -0.025f, 0.0f, 1, 1, 1, 1,
    0.2f, 0.0f, 1, 1, 1, 1,
    -0.3f, -0.1f, 1, 1, 1, 1
  };
  std::vector<unsigned int> indices = {
    0, 1, 2,
    1, 2, 3
  };
  //Test raw
  std::vector<float> rawVbo1 = {
    0.0f, -0.5f, 1, 1, 1, 1,
    0.2f, -0.25f, 1, 1, 1, 1,
    -0.2f, -0.25f, 1, 1, 1, 1,
  };
  std::vector<unsigned int> indices1 = {
    0, 1, 2,
  };

  renderer.RawLayout({
      hobot::LayoutElement(hobot::Type::Float, 2),
      hobot::LayoutElement(hobot::Type::Float, 4),
  });
  renderer.FragShader(hobot::Renderer::DefaultFragShader, false, false);
  renderer.VertShader(hobot::Renderer::DefaultVertShader, false, false);

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

    renderer.Raw(rawVbo.data(), rawVbo.size()*sizeof(float), indices);
    renderer.Raw(rawVbo1.data(), rawVbo1.size()*sizeof(float), indices1);

    glm::vec2 pos1 = {0.2f, 0.5f};
    glm::vec2 pos2 = {0.025f, 0.0f};
    glm::vec2 pos3 = {-0.2f, 0.0f};
    glm::vec2 pos4 = {0.3f, -0.1f};
    renderer.Quad(pos1, pos2, pos3, pos4, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f});
    //renderer.Quad({0, 0}, {0, 1}, pos3, pos4, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, {0.4f, 0.4f, 0.4f, 1.0f}, true); //OrderedMode testing
    //renderer.Circle((pos1+pos2+pos3+pos4)/4.0f, 0.01f, 3, {0.8f, 0.8f, 0.8f, 1.0f}); //TODO: this toggles raw draw (bad)

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
