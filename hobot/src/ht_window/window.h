#pragma once

#include<string>
#include<functional>

#include"ht_api.h"
#include"ht_renderer/renderer.h"
#include"ht_keys.h"
#include"ht_window/windowprops.h"

namespace hobot{

//Thin Window wrapper
class HOBOT_API Window final{
private:
  WindowProps _props;
  std::unique_ptr<Renderer> _pRenderer;
public:
  Window(Window&) = delete;
  Window& operator=(Window&) = delete;
  Window& operator=(Window&&) = delete;
  Window(Window&&) = delete;

  inline int Width() const noexcept { return _props.width; }
  inline int Height() const noexcept { return _props.height; }
  inline std::string Name() const noexcept { return _props.name; }
  const Renderer& GetRenderer() const { return *_pRenderer; } //GetRenderer because of C++ whining about prefixing class

  //Note that -1 apiVersion is the default (e.g. OpenGL - 3.3)
  Window(WindowProps props = {960, 540, "default", -1, false});
  ~Window();

  void Bind(); //Binds the context, no need to call unless multithreaded context
  void SetCallback(Key, std::function<void()> callback); //Overwrites callbacks, and notice that callback is called on repeat
  void DelCallback(Key);
  bool ShouldTerminate(bool should = false); //Getter and setter
  void SwapBuffers();
  void PollEvents();

  void SetProps(WindowProps props);
  void SetVSync(bool enabled);

};

}
