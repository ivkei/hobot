#include"ht_pch/pch.h"

#include"ht_window/window.h"

#include"GLFW/glfw3.h"

#include"ht_logger.h"

namespace hobot{

struct Window::Impl{
  std::unordered_map<int, std::function<void()>> glfwKeyToFunc;
  std::unordered_map<int, std::function<void()>> glfwMButtonToFunc;
  GLFWwindow* pWindow;
};

//Associate window instances with GLFWwindow to retrieve within callbacks (if needed)
static std::unordered_map<GLFWwindow*, Window*> g_glfwWindowToWindow;

#define MAX_KEYS 1024
//PImpl not used as global callback functions need to access those
static std::unordered_map<GLFWwindow*, bool[MAX_KEYS]> g_glfwKeysHeld;

#define MAX_MBUTTONS 32
//Ditto for pImpl
static std::unordered_map<GLFWwindow*, bool[MAX_MBUTTONS]> g_glfwMButtonsHeld;

//Returns -1 if invalid
//For shifted, maps to base
static int KeyToGLFW(Key key){
  switch (key){
    case Key::A: return GLFW_KEY_A;
    case Key::B: return GLFW_KEY_B;
    case Key::C: return GLFW_KEY_C;
    case Key::D: return GLFW_KEY_D;
    case Key::E: return GLFW_KEY_E;
    case Key::F: return GLFW_KEY_F;
    case Key::G: return GLFW_KEY_G;
    case Key::H: return GLFW_KEY_H;
    case Key::I: return GLFW_KEY_I;
    case Key::J: return GLFW_KEY_J;
    case Key::K: return GLFW_KEY_K;
    case Key::L: return GLFW_KEY_L;
    case Key::M: return GLFW_KEY_M;
    case Key::N: return GLFW_KEY_N;
    case Key::O: return GLFW_KEY_O;
    case Key::P: return GLFW_KEY_P;
    case Key::Q: return GLFW_KEY_Q;
    case Key::R: return GLFW_KEY_R;
    case Key::S: return GLFW_KEY_S;
    case Key::T: return GLFW_KEY_T;
    case Key::U: return GLFW_KEY_U;
    case Key::V: return GLFW_KEY_V;
    case Key::W: return GLFW_KEY_W;
    case Key::X: return GLFW_KEY_X;
    case Key::Y: return GLFW_KEY_Y;
    case Key::Z: return GLFW_KEY_Z;

    case Key::LSHIFT: return GLFW_KEY_LEFT_SHIFT;
    case Key::RSHIFT: return GLFW_KEY_RIGHT_SHIFT;
    case Key::LCTRL:  return GLFW_KEY_LEFT_CONTROL;
    case Key::RCTRL:  return GLFW_KEY_RIGHT_CONTROL;
    case Key::LALT:   return GLFW_KEY_LEFT_ALT;
    case Key::RALT:   return GLFW_KEY_RIGHT_ALT;
    case Key::TAB:    return GLFW_KEY_TAB;
    case Key::SUPER:  return GLFW_KEY_LEFT_SUPER;
    case Key::SPC:    return GLFW_KEY_SPACE;
    case Key::ESC:    return GLFW_KEY_ESCAPE;
    case Key::ENTER:  return GLFW_KEY_ENTER;
    case Key::BCSPC:  return GLFW_KEY_BACKSPACE;

    case Key::RIGHT: return GLFW_KEY_RIGHT;
    case Key::UP:    return GLFW_KEY_UP;
    case Key::LEFT:  return GLFW_KEY_LEFT;
    case Key::DOWN:  return GLFW_KEY_DOWN;

    case Key::N1: return GLFW_KEY_1;
    case Key::N2: return GLFW_KEY_2;
    case Key::N3: return GLFW_KEY_3;
    case Key::N4: return GLFW_KEY_4;
    case Key::N5: return GLFW_KEY_5;
    case Key::N6: return GLFW_KEY_6;
    case Key::N7: return GLFW_KEY_7;
    case Key::N8: return GLFW_KEY_8;
    case Key::N9: return GLFW_KEY_9;
    case Key::N0: return GLFW_KEY_0;

    case Key::F1:  return GLFW_KEY_F1;
    case Key::F2:  return GLFW_KEY_F2;
    case Key::F3:  return GLFW_KEY_F3;
    case Key::F4:  return GLFW_KEY_F4;
    case Key::F5:  return GLFW_KEY_F5;
    case Key::F6:  return GLFW_KEY_F6;
    case Key::F7:  return GLFW_KEY_F7;
    case Key::F8:  return GLFW_KEY_F8;
    case Key::F9:  return GLFW_KEY_F9;
    case Key::F10: return GLFW_KEY_F10;
    case Key::F11: return GLFW_KEY_F11;
    case Key::F12: return GLFW_KEY_F12;

    case Key::SEMICOLON: return GLFW_KEY_SEMICOLON;
    case Key::QUOTE:     return GLFW_KEY_APOSTROPHE;
    case Key::COMMA:     return GLFW_KEY_COMMA;
    case Key::PERIOD:    return GLFW_KEY_PERIOD;
    case Key::SLASH:     return GLFW_KEY_SLASH;
    case Key::BCSLASH:   return GLFW_KEY_BACKSLASH;
    case Key::MINUS:     return GLFW_KEY_MINUS;
    case Key::LBPAR:     return GLFW_KEY_LEFT_BRACKET;
    case Key::RBPAR:     return GLFW_KEY_RIGHT_BRACKET;
    case Key::EQ:        return GLFW_KEY_EQUAL;
  }
}

//Returns -1 if invalid
static int MButtonToGLFW(MButton button){
  switch (button){
    case MButton::LEFT:     return GLFW_MOUSE_BUTTON_LEFT;
    case MButton::RIGHT:    return GLFW_MOUSE_BUTTON_RIGHT;
    case MButton::MIDDLE:   return GLFW_MOUSE_BUTTON_MIDDLE;
    default:                return -1;
  }
}

//Otherwise, when functions are called here, jerky motions due to OS specific timings, better to call in PollEvents
static void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods){
  auto& glfwKeysHeld = g_glfwKeysHeld[pWindow];

  if (action == GLFW_PRESS){
    glfwKeysHeld[key] = true;
  } else if (action == GLFW_RELEASE){
    glfwKeysHeld[key] = false;
  }
}

static void MButtonCallback(GLFWwindow* pWindow, int button, int action, int mods){
  auto& glfwMButtonsHeld = g_glfwMButtonsHeld[pWindow];

  if (action == GLFW_PRESS){
    glfwMButtonsHeld[button] = true;
  } else if (action == GLFW_RELEASE){
    glfwMButtonsHeld[button] = false;
  }
}

//Just define them here for now
Window::Window(WindowProps props)
: _props(props), _pImpl(std::make_unique<Window::Impl>()){
  //Init glfw
  if (!glfwInit()){
    HT_LOG_ERROR("Couldnt init glfw");
    std::terminate();
  }

  //Settings (default version)
  if (props.apiVersion == -1){
    props.apiVersion = 4.4f;
    _props.apiVersion = 4.4f;
  }

  int major = static_cast<int>(std::floor(props.apiVersion));
  //Avoid the inaccurate float representation
  int minor = static_cast<int>(std::ceil((props.apiVersion - std::floor(props.apiVersion))*10));

  HT_LOG_INFO("Version: ", major, ".", minor, " Passed: ", props.apiVersion);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Core profile
  //Disables backward compatibility with Legacy OpenGL
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For Mac

  glfwWindowHint(GLFW_RESIZABLE, props.resizable);

  HT_LOG_INFO("Creating GLFW window...");

  //Create window with its OpenGL context
  GLFWwindow* pWindow = glfwCreateWindow(props.width, props.height, props.name.c_str(), NULL, NULL); //NULLs for fullscreen and sharing context
  g_glfwWindowToWindow.emplace(pWindow, this);
  _pImpl->pWindow = pWindow;
  if (!pWindow){
    HT_LOG_ERROR("Couldnt create window (glfw)");
    glfwTerminate();
    std::terminate();
  }

  HT_LOG_INFO("Made GLFW window");

  //Make windows context current
  glfwMakeContextCurrent(pWindow);

  glfwSetKeyCallback(pWindow, KeyCallback);

  HT_LOG_INFO("OpenGL Context is set");

  _pRenderer = std::make_unique<class Renderer>(props);
  if (!_pRenderer->IsValid()){
    HT_LOG_ERROR("Renderer wasnt init, setting should terminate");
    this->ShouldTerminate(true);
  }

  _pRenderer->SetViewport({0, 0}, {1, 1});
  //Viewport has to be adjusted for whenever the window is resized
  glfwSetFramebufferSizeCallback(pWindow, [](GLFWwindow* pWindow, int width, int height){
    Window* pWindowWrap = g_glfwWindowToWindow[pWindow];

    pWindowWrap->SetProps({width, height, pWindowWrap->Name(), -1, true});
    const auto& renderer = pWindowWrap->GetRenderer();
    Vec4 oldViewport = renderer.GetViewport();
    renderer.SetViewport({oldViewport.x, oldViewport.y}, {oldViewport.z, oldViewport.w}); //The viewport is still the same, just need to update it
  });

  //Set the keys to false
  for (bool& i : g_glfwKeysHeld[pWindow]){
    i = false;
  }

  //Mouse
  for (bool& i : g_glfwMButtonsHeld[pWindow]){
    i = false;
  }
  glfwSetMouseButtonCallback(pWindow, MButtonCallback);

  HT_LOG_SUCCESS("Window constructed");
}

Window::~Window(){
  g_glfwWindowToWindow.erase(_pImpl->pWindow);
  glfwTerminate();
  //Clean all glfw's allocated resources, its made in c :(
}

void Window::Bind(){
  glfwMakeContextCurrent(_pImpl->pWindow);
}

void Window::SetCallback(Key key, std::function<void()> callback){
  int glfwKey = KeyToGLFW(key);
  HT_LOG_ASSERT(glfwKey != -1, "Invalid key passed to Window::SetCallback");

  if (_pImpl->glfwKeyToFunc.contains(glfwKey)){
    _pImpl->glfwKeyToFunc.erase(glfwKey);
  }
  _pImpl->glfwKeyToFunc.emplace(glfwKey, callback);
}
void Window::DelCallback(Key key){
  int glfwKey = KeyToGLFW(key);
  HT_LOG_ASSERT(glfwKey != -1, "Invalid key passed to Window::DelCallback");

  if (_pImpl->glfwKeyToFunc.contains(glfwKey)){
    _pImpl->glfwKeyToFunc.erase(glfwKey);
  }
}
bool Window::ShouldTerminate(bool should){
  if (should) glfwSetWindowShouldClose(_pImpl->pWindow, true);
  return glfwWindowShouldClose(_pImpl->pWindow);
}
void Window::SwapBuffers(){
  glfwSwapBuffers(_pImpl->pWindow);
}
void Window::PollEvents(){
  glfwPollEvents();

  //Handle key callbacks
  for (auto&& i : _pImpl->glfwKeyToFunc){
    if (g_glfwKeysHeld[_pImpl->pWindow][i.first]) i.second();
  }

  //Handle mbutton callbacks
  for (auto&& i : _pImpl->glfwMButtonToFunc){
    if (g_glfwMButtonsHeld[_pImpl->pWindow][i.first]) i.second();
  }
}

void Window::SetProps(WindowProps props){
  this->_props = props;
  glfwSetWindowSize(_pImpl->pWindow, props.width, props.height);
  glfwSetWindowTitle(_pImpl->pWindow, props.name.c_str());
  glfwWindowHint(GLFW_RESIZABLE, props.resizable);

  _pRenderer->_SetWindowProps(props);
}

void Window::SetVSync(bool enabled){
  glfwSwapInterval(enabled);
}
//Works through delaying glfwSwapBuffers(window) so that swap happens once every monitor draw finishes
//If GPU cant keep up, will drop down to 60->30->...

bool Window::IsKeyPressed(Key key){
  int glfwKey = KeyToGLFW(key);
  HT_LOG_ASSERT(glfwKey != -1, "Invalid key passed to IsKeyPressed!");

  return g_glfwKeysHeld[_pImpl->pWindow][glfwKey];
}

Vec2 Window::MousePos(){
  double x;
  double y;
  glfwGetCursorPos(_pImpl->pWindow, &x, &y);
  return Vec2(x, y);
}

bool Window::IsMButtonPressed(MButton button){
  //Depends on PollEvents (mouse can be pressed and released in between of 2 calls, and this wont register that)
  int glfwButton = MButtonToGLFW(button);
  return glfwGetMouseButton(_pImpl->pWindow, glfwButton) == GLFW_PRESS;
}
void Window::SetCallback(MButton button, std::function<void()> callback){
  int glfwButton = MButtonToGLFW(button);
  HT_LOG_ASSERT(glfwButton != -1, "Invalid button passed to Window::SetCallback");

  if (_pImpl->glfwMButtonToFunc.contains(glfwButton)){
    _pImpl->glfwMButtonToFunc.erase(glfwButton);
  }
  _pImpl->glfwMButtonToFunc.emplace(glfwButton, callback);
}

void Window::DelCallback(MButton button){
  int glfwButton = MButtonToGLFW(button);
  HT_LOG_ASSERT(glfwButton != -1, "Invalid button passed to Window::DelCallback");

  if (_pImpl->glfwMButtonToFunc.contains(glfwButton)){
    _pImpl->glfwMButtonToFunc.erase(glfwButton);
  }
}

}

