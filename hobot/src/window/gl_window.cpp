#include"pch/pch.h"

#include"window.h"

#include"GLFW/glfw3.h"

#include"logger.h"

namespace hobot{

//Associate window instances with GLFWwindow
//No PImpl because then cant utilize glfw callbacks
static std::unordered_map<Window*, GLFWwindow*> addressToWindow;

static std::unordered_map<int, std::function<void()>> glfwKeyToFunc;

//Returns -1 if invalid
static int KeyToGLFW(Key key){
  switch (key){
    case (Key::W): return GLFW_KEY_W;
    case (Key::A): return GLFW_KEY_A;
    case (Key::S): return GLFW_KEY_S;
    case (Key::D): return GLFW_KEY_D;
    case (Key::SPC): return GLFW_KEY_SPACE;
    case (Key::ESC): return GLFW_KEY_ESCAPE;
    defalt: return -1;
  }
}

static void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods){
  if (action == GLFW_PRESS && glfwKeyToFunc.contains(key)){
    glfwKeyToFunc.at(key)();
  }
}

//Just define them here for now
Window::Window(WindowProps props)
: _props(props){
  //Init glfw
  if (!glfwInit()){
    HT_LOG_ERROR("Couldnt init glfw");
    std::terminate();
  }

  //Settings
  if (props.apiVersion == -1){
    props.apiVersion = 3.3f;
    _props.apiVersion = 3.3f;
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
  addressToWindow.emplace(this, pWindow);
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
      auto it = std::find_if(addressToWindow.begin(), addressToWindow.end(), [pWindow](const auto& pair){
        return pair.second == pWindow;
      });
      Window* pWindowWrap = it->first;

      pWindowWrap->SetProps({width, height, pWindowWrap->Name(), -1, true});
      const auto& renderer = pWindowWrap->GetRenderer();
      glm::vec4 oldViewport = renderer.GetViewport();
      renderer.SetViewport({oldViewport.x, oldViewport.y}, {oldViewport.z, oldViewport.w}); //The viewport is still the same, just need to update it
  });

  HT_LOG_SUCCESS("Window constructed");
}

Window::~Window(){
  addressToWindow.erase(this);
  glfwTerminate();
  //Clean all glfw's allocated resources, its made in c :(
}

void Window::Bind(){
  GLFWwindow* pWindow = addressToWindow.at(this);
  glfwMakeContextCurrent(pWindow);
}

void Window::SetCallback(Key key, std::function<void()> callback){
  int glfwKey = KeyToGLFW(key);
  HT_LOG_ASSERT(glfwKey != -1, "Invalid key passed to Window::SetCallback");

  if (glfwKeyToFunc.contains(glfwKey)){
    glfwKeyToFunc.erase(glfwKey);
  }
  glfwKeyToFunc.emplace(glfwKey, callback);
}
void Window::DelCallback(Key key){
  int glfwKey = KeyToGLFW(key);
  HT_LOG_ASSERT(glfwKey != -1, "Invalid key passed to Window::DelCallback");
  if (glfwKeyToFunc.contains(glfwKey)){
    glfwKeyToFunc.erase(glfwKey);
  }
}
bool Window::ShouldTerminate(bool should){
  GLFWwindow* pWindow = addressToWindow.at(this);
  if (should) glfwSetWindowShouldClose(pWindow, true);
  return glfwWindowShouldClose(pWindow);
}
void Window::SwapBuffers(){
  GLFWwindow* pWindow = addressToWindow.at(this);
  glfwSwapBuffers(pWindow);
}
void Window::PollEvents(){
  GLFWwindow* pWindow = addressToWindow.at(this);
  glfwPollEvents();
}

void Window::SetProps(WindowProps props){
  this->_props = props;
  GLFWwindow* pWindow = addressToWindow.at(this);
  glfwSetWindowSize(pWindow, props.width, props.height);
  glfwSetWindowTitle(pWindow, props.name.c_str());
  glfwWindowHint(GLFW_RESIZABLE, props.resizable);

  _pRenderer->_SetWindowProps(props);
}

void Window::SetVSync(bool enabled){
  glfwSwapInterval(enabled);
}
//Works through delaying glfwSwapBuffers(window) so that swap happens once every monitor draw finishes
//If GPU cant keep up, will drop down to 60->30->...
}

