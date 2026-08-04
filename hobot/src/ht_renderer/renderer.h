#pragma once

#include<memory>
#include<functional>

#include"ht_api.h"
#include"ht_window/windowprops.h"
#include"ht_renderer/layoutelement.h"
#include"ht_math/math.h"

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
public:
  Renderer(Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  Renderer(WindowProps props);
  ~Renderer();

  //Textures (dont bind more than allowed at once, query MaxTextures, note that indexing starts with 0)
  void SetTexture(std::string path, std::string name, bool generateMipmaps = true) const; //Both overwrite a texture if name is repeated
  void SetTexture(unsigned int width, unsigned int height, std::string name) const;
  void BindTexture(std::string name, unsigned int slot = 0, bool image = false) const;
  void ClearTexture(std::string name, hobot::Vec4 color = hobot::Vec4(1)) const;
  int MaxTextures() const; //Read-only, optimized for drawing
  int MaxImages() const; //Read and write

  void Render() const;
  void Clear(hobot::Vec4 color = hobot::Vec4(0, 0, 0, 1)) const;

  bool IsValid() const;
  //Both start and dimensions are between 0 and 1, thats because its independent of window's size
  void SetViewport(hobot::Vec2 start, hobot::Vec2 dimensions) const; //Setter yet const as its needed to be called from const references
  hobot::Vec4 GetViewport() const;
  void _SetWindowProps(WindowProps props);

  //Custom pipeline
  //Notice that it accumulates, and on render clears (no need to adjust indices for past objects, treat each as first)
  //Size is for data's size in bytes
  void Raw(const void* data, unsigned int size, const std::vector<unsigned int>& indices) const;
  void RawLayout(const std::vector<LayoutElement>& layout) const;

  static const char* DefaultFragShader;
  static const char* DefaultVertShader;

  void Uniform(const char* name, int i, bool fixed = true) const;
  void Uniform(const char* name, float f, bool fixed = true) const;
  void Uniform(const char* name, hobot::Mat4 m, bool fixed = true) const;
  void Uniform(const char* name, hobot::Vec4 v, bool fixed = true) const;
  void Uniform(const char* name, hobot::Vec2 v, bool fixed = true) const;

  //Interprets first arg as source if second is false, otherwise parses a file via a file path, use hobot::GetExecDir() if needed
  //Path is relative to exec file
  //Fixed is for pre-made functions such as trig, fixed=false implies shader source for Raw()
  //Make recompile=false in case you dont want to recompile after changing shader
  //Shaders are set to default by default
  void FragShader(const char* string = DefaultFragShader, bool isPath = false, bool fixed = true, bool recompile = true) const;
  void VertShader(const char* string = DefaultVertShader, bool isPath = false, bool fixed = true, bool recompile = true) const;

  //Use this if specifying both, otherwise errors are given as its trying to recompile with incompatible
  void Shaders(const char* vStr = DefaultVertShader, const char* fStr = DefaultFragShader, bool vIsPath = false, bool fIsPath = false, bool fixed = true) const;

  //"Fixed-function pipeline"

  //pos = bottom-left vertex pos, dimensions = width, height
  void Quad(hobot::Vec2 pos, hobot::Vec2 dimensions, hobot::Vec4 color = hobot::Vec4(1)) const;
  //Ordered mode ensures that the order of vertices doesnt matter; however, since every quad has at most 2 diagonals (for shapes less it will do fine),
  //it will possibly render unexpected shape if a weird shape was passed to it
  //Proper order follows a pattern of 2 trigs where
  //  -Trig1 is vertex0, vertex1, and vertex2
  //  -Trig2 is vertex1, vertex2, and vertex3
  //If its improper (that is it doesnt follow such a pattern), weird "rectangles" can be drawn where one overlaps the other
  void Quad(hobot::Vec2 vertexPos0, hobot::Vec2 vertexPos1, hobot::Vec2 vertexPos2, hobot::Vec2 vertexPos3,
            hobot::Vec4 vertexColor0, hobot::Vec4 vertexColor1, hobot::Vec4 vertexColor2, hobot::Vec4 vertexColor3, bool orderedMode = false) const;

  //pos = bottom-left vertex pos, dimensions = base width, height, triangle = right
  void Trig(hobot::Vec2 pos, hobot::Vec2 dimensions, hobot::Vec4 color = hobot::Vec4(1)) const;
  void Trig(hobot::Vec2 vertexPos0, hobot::Vec2 vertexPos1, hobot::Vec2 vertexPos2,
            hobot::Vec4 vertexColor0, hobot::Vec4 vertexColor1, hobot::Vec4 vertexColor2) const;

  //It draws regular polygons
  //rotation in rads
  //pos = center of the circle coordinates
  void Reg(hobot::Vec2 pos, float r, int vertices = 30, hobot::Vec4 color = hobot::Vec4(1), float rotation = 0) const;
  //pos = center of the circle coordinates
  void Reg(hobot::Vec2 pos, float r, int vertices, hobot::Vec4 centerColor, hobot::Vec4 circumferenceColor, float rotation = 0) const;
  //pos = center of the circle coordinates
  void Reg(hobot::Vec2 pos, float r, int vertices, bool isRainbow = false, float rotation = 0) const;

  void SetWireframe(bool enabled);
};

}
