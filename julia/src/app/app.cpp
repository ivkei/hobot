#include"app.h"

#include<chrono>
#include<complex>

#include"glm/gtc/constants.hpp"

App::App()
: _pWindow(std::make_unique<hobot::Window>(hobot::WindowProps(1080, 1080, "julia", -1, false))),
  _pause(false), _pauseTimer(0), _pauseCD(0.2){
  _pWindow->SetVSync(true);
  _pWindow->SetCallback(hobot::Key::SPC, [&](){
    if (this->_pauseTimer >= this->_pauseCD){
      this->_pause = !this->_pause;
      this->_pauseTimer = 0;
    }
  });
}

App::~App(){
}

void App::Run(){
  auto cnow = std::chrono::high_resolution_clock::now;

  const auto& renderer = _pWindow->GetRenderer();

  //DeltaSeconds
  auto lastFrame = cnow();
  float deltaSeconds;

  renderer.Shaders(RES_DIR "shaders/vert.glsl", RES_DIR "shaders/frag.glsl", true, true, true);

  //Interpolate c = e^ia with a ranging from 0 to 2pi
  float timer = 0;
  float maxTimer = 30;

  //Iterations
  int iterations = 50;
  renderer.Uniform("uIterations", iterations);

  //FPS
  float fpsTimer = 0;
  int frames = 0;
  while (!_pWindow->ShouldTerminate()){
    auto now = cnow();
    //From nanoseconds
    deltaSeconds = (now - lastFrame).count()*1e-9;
    lastFrame = cnow();

    //Pause
    _pauseTimer+=deltaSeconds;

    //FPS
    frames++;
    if ((fpsTimer+=deltaSeconds)>=1){
      HT_LOG_INFO("FPS: ", frames);
      frames = 0;
      fpsTimer = 0;
    }

    //Interpolate c
    if (!_pause && (timer+=deltaSeconds) >= maxTimer) timer = 0;
    //Different cs
    float pi = glm::pi<float>();
    float a = glm::mix(0.0f, 2*pi, timer/maxTimer);
    float e = glm::exp(1);
    std::complex<float> i = 0.0f+1.0if;
    std::complex<float> c = 0.7885f*std::pow(e, i*a);

    //std::complex<float> c = -0.8f + 0.156if;
    //std::complex<float> c = -0.7269f + 0.1889if;
    //std::complex<float> c = -0.8f + 0.156if;
    //std::complex<float> c = 0.4f + 0.4if;

    renderer.Uniform("uCReal", c.real());
    renderer.Uniform("uCImag", c.imag());

    //R (Escape radius)
    float R = (1.0f+std::sqrt(1.0f+std::abs(c)))/2.0f;
    renderer.Uniform("uR", R);

    //Fullscreen quad
    renderer.Quad({-1, -1}, {2, 2}, {0, 0, 0, 1}); //Color doesnt atter

    renderer.Render();
    _pWindow->PollEvents();
    _pWindow->SwapBuffers();
  }
}
