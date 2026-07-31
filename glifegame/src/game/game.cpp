#include"game.h"

#include"hobot.h"

hobot::WindowProps Game::GetWindowProps(){
  return {1920, 1080, "glifegame", 4.4, false};
}

static const int WIDTH = 192*4;
static const int HEIGHT = 108*4;
//static const hobot::Vec4 COLORALIVE = {0.85f, 0.08f, 0.2f, 1.0f};
static const hobot::Vec4 COLORALIVE = {0.67f, 0.84f, 0.9f, 1.0f};
static const hobot::Vec4 COLORDEAD = {0.1f, 0.1f, 0.1f, 1.0f};
static const int STARTINGPOPULATION = 40;

float Game::FixedInterval(){
  return 1.0f/165.0f;
  //return 0.1f;
}

Game::Game(const hobot::Renderer& renderer)
: _renderer(renderer),
  _backSlot(0){

  renderer.SetTexture(WIDTH, HEIGHT, "0"); //Where next field is computed based on front
  renderer.SetTexture(WIDTH, HEIGHT, "1"); //Whats drawn

  //Populate
  _renderer.ClearTexture("1", {0.93434f, 0.912412f, 0.123422f, 1.0f});
  renderer.Shaders(RES_DIR"shaders/vert.glsl", RES_DIR"shaders/populateFrag.glsl", true, true, true);
  renderer.BindTexture("1", 0, true);
  renderer.Uniform("uTexture", 0);
  _renderer.Uniform("uWidth", WIDTH);
  _renderer.Uniform("uHeight", HEIGHT);
  _renderer.Uniform("uColorAlive", COLORALIVE);
  _renderer.Uniform("uColorDead", COLORDEAD);
  _renderer.Uniform("uPopulation", STARTINGPOPULATION);
  _renderer.Uniform("uSeed", (int)time(NULL));
  renderer.Clear();
  renderer.Quad({-1, -1}, {2, 2});
  renderer.Render();

  //Shaders
  renderer.Shaders(RES_DIR"shaders/vert.glsl", RES_DIR"shaders/mainFrag.glsl", true, true, true);

  //Set uniforms
  _renderer.Uniform("uTextureBack", 0);
  _renderer.Uniform("uTextureFront", 1);
  _renderer.Uniform("uWidth", WIDTH);
  _renderer.Uniform("uHeight", HEIGHT);
  _renderer.Uniform("uColorAlive", COLORALIVE);
  _renderer.Uniform("uColorDead", COLORDEAD);

}

void Game::OnUpdate(float deltaSeconds){
}

void Game::OnFixedUpdate(float deltaSeconds){
  //Clear the back
  _renderer.ClearTexture(std::to_string(_backSlot), {0.1f, 0.52342f, 0.123422f, 0.012424f});

  //Swap textures
  unsigned int frontSlot = (_backSlot+1)%2;
  _renderer.BindTexture("0", _backSlot, true); //As images
  _renderer.BindTexture("1", frontSlot, true);
  _backSlot = frontSlot;

  _renderer.Clear();

  //Fullscreen quad (color doesnt matter)
  _renderer.Quad({-1, -1}, {2, 2}, {1, 0, 1, 1});

  _renderer.Render();
  _toSwapBuffers = true;
}

Game::~Game(){
}
