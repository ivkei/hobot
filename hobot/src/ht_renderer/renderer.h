#pragma once

#include<memory>
#include<functional>
#include<array>

#include"ht_api.h"
#include"ht_window/windowprops.h"
#include"ht_renderer/layoutelement.h"
#include"ht_math/math.h"

#include"ht_renderer/renderable/renderable.h"

namespace hobot{

//Batch renderer
//Batch via functions, Render draws
//Draws on a bound window
//Use through window
//Dont create it, use it through window
class HOBOT_API Renderer final{
private:
  //For specific API variables
  struct PImpl;
  std::unique_ptr<PImpl> _pImpl;
  WindowProps _props; //Useful to have here

  bool _valid;
public:
  Renderer(Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(WindowProps props);
  ~Renderer();

  //===Render (Flushing) & Clear===
  void Render() const;
  void Clear(hobot::Vec4 color = hobot::Vec4(0, 0, 0, 1)) const;
  void Submit(const std::vector<VertexData>& data);

  //===Viewport & IsValid===
  //API initialized
  bool IsValid() const;
  //Both start and dimensions are between 0 and 1, thats because its independent of window's size
  void SetViewport(hobot::Vec2 start, hobot::Vec2 dimensions) const; //Setter yet const as its needed to be called from const references
  hobot::Vec4 GetViewport() const;
  void _SetWindowProps(const WindowProps& props);

  //===Sprites & Textures===

  //Textures (dont bind more than allowed at once, query MaxTextures, note that indexing starts with 0)
  //TODO:Path to image?
  void SetTexture(std::string path, std::string name, bool generateMipmaps = true) const; //Both overwrite a texture if name is repeated
  void SetTexture(unsigned int width, unsigned int height, std::string name) const;
  void BindTexture(std::string name, unsigned int slot = 0, bool image = false) const;
  void ClearTexture(std::string name, hobot::Vec4 color = hobot::Vec4(1)) const;
  int MaxTextures() const; //Read-only, optimized for drawing
  int MaxImages() const; //Read and write

  static const char* DefaultFragShader;
  static const char* DefaultVertShader;

  void Uniform(const char* name, int i) const;
  void Uniform(const char* name, float f) const;
  void Uniform(const char* name, hobot::Mat4 m) const;
  void Uniform(const char* name, hobot::Vec4 v) const;
  void Uniform(const char* name, hobot::Vec2 v) const;

  //===Shaders===

  //Interprets first arg as source if second is false, otherwise parses a file via a file path, uses hobot::ReadRel()
  //Path is relative to exec file
  //Fixed is for pre-made functions such as trig, fixed=false implies shader source for Raw()
  //Make recompile=false in case you dont want to recompile after changing shader
  //Shaders are set to default by default for fixed and Sprite pipelines ONLY!
  void FragShader(const char* string = DefaultFragShader, bool isPath = false, bool recompile = true) const;
  void VertShader(const char* string = DefaultVertShader, bool isPath = false, bool recompile = true) const;

  //Use this if specifying both, otherwise errors are given as its trying to recompile with incompatible
  //Using this is highly advised!
  void Shaders(const char* vStr = DefaultVertShader, const char* fStr = DefaultFragShader, bool vIsPath = false, bool fIsPath = false, Pipeline pipeline = Pipeline::Fixed) const;

  //===Utils===
  void SetWireframe(bool enabled);
};

}
