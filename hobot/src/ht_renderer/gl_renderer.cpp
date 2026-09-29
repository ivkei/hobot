#include"ht_pch/pch.h"

#include"ht_renderer/renderer.h"
#include"ht_renderer/renderbuffer/renderbuffer.h"

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/shader/shader.h"
#include"ht_glutils/vao/vao.h"
#include"ht_glutils/texture/texture.h"

namespace hobot{

struct Vertex{
  hobot::Vec2 pos;
  hobot::Vec4 color;
};

//Note that this defines layout for spriteVbo
//In renderer's ctor
struct SpriteVertex{
  Vec2 pos;
  Vec4 col;
  Vec2 texCoord;
  int sprite;
};

struct Renderer::PImpl{
  //Fixed
  std::vector<Vertex> fixedVbo;
  std::vector<unsigned int> fixedIbo;
  Shader fixedShader;

  RenderBuffer fixedBuffer{RenderBuffer::BufferType::Dynamic};
  //Raw
  void* pRawData = nullptr;
  unsigned int rawSize = 0;
  std::vector<unsigned int> rawIbo;

  std::vector<LayoutElement> rawLayout;

  //For proper automatic index handling
  unsigned int rawMaxIndex = 0; //1-indexed!

  Shader rawShader;

  RenderBuffer rawBuffer{RenderBuffer::BufferType::Dynamic};

  //Else
  bool clear = false;
  hobot::Vec4 viewport;

  //Textures
  std::unordered_map<std::string, Texture> textures;
  int maxTextureSlots;
  int maxImageSlots;

  //Sprites
  std::unordered_map<std::string, std::pair<std::shared_ptr<Texture>, int>> spriteTextureCache; //cache textures with their lifetime counter by path
  int maxTextureSpriteCacheLifetime = 1; //How much renders it stays cached for, e.g. 1 implies that will get deleted on next render after
  Shader spriteShader;
  std::vector<SpriteVertex> spriteVbo;
  std::vector<unsigned int> spriteIbo;
  std::vector<std::shared_ptr<Texture>> sprites; //This keeps the sprites for spriteVbo, intex of texture pointer implies the sampler number (take mod)
};

void Renderer::SetTexture(std::string path, std::string name, bool generateMipmaps) const{
  if (_pImpl->textures.contains(name)) _pImpl->textures.erase(name);
  _pImpl->textures.emplace(name, Texture(path, generateMipmaps));
}
void Renderer::SetTexture(unsigned int width, unsigned int height, std::string name) const{
  if (_pImpl->textures.contains(name)) _pImpl->textures.erase(name);
  _pImpl->textures.emplace(name, Texture(width, height));
}
void Renderer::BindTexture(std::string name, unsigned int slot, bool image) const{
  HT_LOG_ASSERT(slot < _pImpl->maxTextureSlots || !image, "Texture is bound to a slot that isnt supported!");
  HT_LOG_ASSERT(slot < _pImpl->maxImageSlots || image, "Image is bound to a slot that isnt supported!");
  HT_LOG_ASSERT(_pImpl->textures.contains(name), "There's no texture/image called '", name, "' that was specified via SetTexture (in BindTexture)");
  _pImpl->textures.at(name).Bind(slot, image);
}
int Renderer::MaxTextures() const{
  return _pImpl->maxTextureSlots;
}
int Renderer::MaxImages() const{
  return _pImpl->maxImageSlots;
}
void Renderer::ClearTexture(std::string name, hobot::Vec4 color) const{
  HT_LOG_ASSERT(_pImpl->textures.contains(name), "There's no texture/image called '", name, "' that was specified via SetTexture (in ClearTexture)");
  _pImpl->textures.at(name).Clear(color);
}

//Note that theres no need for multithreadedness, window has ownership and it can be bound only to thread at a time
Renderer::Renderer(WindowProps props)
: _props(props), _valid(true){
  HT_LOG_INFO("Creating Renderer...");

  //===GLEW initialization===

  //Window initializes the context
  auto err = glewInit();
  if (err != GLEW_OK){
    HT_LOG_ERROR("Failed to init glew: ", glewGetErrorString(err));
    _valid = false;
    return;
  }

  _pImpl = std::make_unique<PImpl>();

  GLEnableAutoLogging();

  GLCall(HT_LOG_INFO("Initialized glew (version): ", glGetString(GL_VERSION)));

  //Blending
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glEnable(GL_BLEND));

  //===Post-GLEW initialization===

  //RenderBuffers
  _pImpl->fixedBuffer.SetLayout({{Type::Float, 2}, {Type::Float, 4}});

  //Shaders
  HT_LOG_INFO("---Current default fixed vert shader---\n", this->DefaultFixedVertShader);
  HT_LOG_INFO("---Current default fixed frag shader---\n", this->DefaultFixedFragShader);

  //Fixed
  this->Shaders(DefaultFixedVertShader, DefaultFixedFragShader, false, false, Pipeline::Fixed);

  //Textures
  GLCall(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_pImpl->maxTextureSlots));
  HT_LOG_INFO("Max texture slots: ", _pImpl->maxTextureSlots);

  GLCall(glGetIntegerv(GL_MAX_IMAGE_UNITS, &_pImpl->maxImageSlots));
  HT_LOG_INFO("Max image slots: ", _pImpl->maxImageSlots);

  //Sprite
  //this->Shaders(DefaultSpriteVertShader, DefaultSpriteFragShader, false, false, Pipeline::Sprite); //TODO: uncomment
  VBOLayout spriteLayout;
  spriteLayout.Push<float>(2); //Pos
  spriteLayout.Push<float>(4); //Color
  spriteLayout.Push<float>(2); //TexCoord
  spriteLayout.Push<int>(1); //Sampler
  //_pImpl->spriteVao.AddLayout(spriteLayout); //TODO: add sprite support
}

Renderer::~Renderer(){
  if (_pImpl->pRawData){
    free(_pImpl->pRawData);
  }
}

//Buffers data every frame, but otherwise too complex
void Renderer::Render() const{
  if (_pImpl->clear){
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
  }

  if (_pImpl->rawIbo.empty() && _pImpl->fixedIbo.empty() && _pImpl->spriteIbo.empty()) return;

  //Fixed pipeline rendering
  if (_pImpl->fixedIbo.size() > 2){
    HT_LOG_ASSERT(_pImpl->fixedShader.IsValid(), "Fixed shader is invalid, please specify it with Vert and Frag");
    _pImpl->fixedBuffer.Vertex(_pImpl->fixedVbo.data(), _pImpl->fixedVbo.size()*sizeof(Vertex));
    _pImpl->fixedBuffer.Index(_pImpl->fixedIbo.data(), _pImpl->fixedIbo.size()*sizeof(unsigned int));

    _pImpl->fixedBuffer.Bind();
    _pImpl->fixedShader.Bind();

    glDrawElements(GL_TRIANGLES, _pImpl->fixedIbo.size(), GL_UNSIGNED_INT, NULL);

    _pImpl->fixedBuffer.Unbind();
    _pImpl->fixedShader.Unbind();

    //Next batch prep
    _pImpl->fixedVbo.clear();
    _pImpl->fixedIbo.clear();
  }

  //Raw draw
  if (_pImpl->rawIbo.size() > 2 && _pImpl->pRawData){
    HT_LOG_ASSERT(_pImpl->rawShader.IsValid(), "Raw shader is invalid, please specify it with Vert and Frag");
    _pImpl->rawBuffer.Vertex(_pImpl->pRawData, _pImpl->rawSize);
    _pImpl->rawBuffer.Index(_pImpl->rawIbo.data(), _pImpl->rawIbo.size()*sizeof(unsigned int));

    _pImpl->rawBuffer.Bind();
    _pImpl->rawShader.Bind();

    _pImpl->rawBuffer.SetLayout(_pImpl->rawLayout);

    glDrawElements(GL_TRIANGLES, _pImpl->rawIbo.size(), GL_UNSIGNED_INT, NULL);

    _pImpl->rawBuffer.Unbind();
    _pImpl->rawShader.Unbind();

    //Next batch prep
    _pImpl->rawSize = 0;
    _pImpl->rawIbo.clear();
    _pImpl->rawMaxIndex = 0;
  }
  /*Sprites (in batches of whatever number allowed)
  if (spriteIbo.size()){
    spriteShader.Bind();
    spriteVao.Bind();
    //TODO
  }
  */

  //Prepare for the next batch
  this->_pImpl->clear = false;
  GLCall(glClearColor(0, 0, 0, 1));

  /*Sprites
  _pImpl->spriteIbo.clear();
  _pImpl->spriteVbo.clear();
  sprites.clear();
  if (_pImpl->spriteTextureCache.size()){
    for (auto&& i : _pImpl->spriteTextureCache){
      if (i.second.second-- <= 0) _pImpl->spriteTextureCache.erase(i.first);
    }
  }

  */
}

void Renderer::Submit(const std::vector<VertexData>& data){
  //TODO: through renderbuffers or glMapBuffer?
}

//pos = bottom-left vertex pos, dimensions = width, height
void Renderer::Quad(hobot::Vec2 pos, hobot::Vec2 dimensions, hobot::Vec4 color) const{
  this->Quad(pos, {pos.x+dimensions.x, pos.y}, {pos.x, pos.y+dimensions.y}, {pos.x+dimensions.x, pos.y+dimensions.y}, color, color, color, color, false);
}
static float AtFor2Pts(float x, hobot::Vec2 p1, hobot::Vec2 p2){
  return (p1.x != p2.x) && (p1.x*p2.x - p1.y*p2.y-x*(p2.y-p1.y))/(p1.x-p2.x);
}
//If 2 points divide the remaining 2 points so that they are on opposite sides of a diagonal, those 2 endpts can be used to draw 2 triangles no matter what order
//This orders the indices to disregard invalid order of input
#define IfOrder(endpt1, endpt2, other1, other2)\
if ((pos##other1.y > AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y < AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2)) ||\
    (pos##other1.y < AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y > AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2))){\
  orderedIndices[0] = other1;\
  orderedIndices[1] = endpt1;\
  orderedIndices[2] = endpt2;\
  orderedIndices[3] = other2;\
}

#define PushIboQuad()\
  if (orderedMode){\
    int orderedIndices[4] = {0, 1, 2, 3};\
    IfOrder(0, 1, 2, 3)\
    else IfOrder(0, 2, 1, 3)\
    else IfOrder(1, 2, 0, 3)\
    else IfOrder(0, 3, 1, 2)\
    else IfOrder(1, 3, 0, 2)\
    else IfOrder(3, 2, 0, 1);\
    ibo.push_back(offset+orderedIndices[0]);\
    ibo.push_back(offset+orderedIndices[1]);\
    ibo.push_back(offset+orderedIndices[2]);\
    ibo.push_back(offset+orderedIndices[1]);\
    ibo.push_back(offset+orderedIndices[2]);\
    ibo.push_back(offset+orderedIndices[3]);\
  } else{\
    ibo.push_back(offset+0);\
    ibo.push_back(offset+1);\
    ibo.push_back(offset+2);\
    ibo.push_back(offset+1);\
    ibo.push_back(offset+2);\
    ibo.push_back(offset+3);\
  }

void Renderer::Quad(hobot::Vec2 pos0, hobot::Vec2 pos1, hobot::Vec2 pos2, hobot::Vec2 pos3,
                    hobot::Vec4 col0, hobot::Vec4 col1, hobot::Vec4 col2, hobot::Vec4 col3, bool orderedMode) const{
  auto& vbo = _pImpl->fixedVbo;
  auto& ibo = _pImpl->fixedIbo;

  int offset = vbo.size();
  //Vbo
  vbo.emplace_back(Vertex{pos0, col0});
  vbo.emplace_back(Vertex{pos1, col1});
  vbo.emplace_back(Vertex{pos2, col2});
  vbo.emplace_back(Vertex{pos3, col3});

  //Ibo
  PushIboQuad();
}

//pos = bottom-left vertex pos, dimensions = base width, height, triangle = right
void Renderer::Trig(hobot::Vec2 pos, hobot::Vec2 dimensions, hobot::Vec4 color) const{
  this->Trig(pos, {pos.x+dimensions.x, pos.y}, {pos.x, pos.y + dimensions.y}, color, color, color);
}

void Renderer::Trig(hobot::Vec2 pos0, hobot::Vec2 pos1, hobot::Vec2 pos2,
          hobot::Vec4 col0, hobot::Vec4 col1, hobot::Vec4 col2) const{
  auto& vbo = _pImpl->fixedVbo;
  auto& ibo = _pImpl->fixedIbo;
  int offset = vbo.size();
  //Vbo
  vbo.emplace_back(Vertex{pos0, col0});
  vbo.emplace_back(Vertex{pos1, col1});
  vbo.emplace_back(Vertex{pos2, col2});

  //Ibo
  ibo.push_back(offset);
  ibo.push_back(offset+1);
  ibo.push_back(offset+2);

}

void Renderer::Reg(hobot::Vec2 pos, float r, int vertices, hobot::Vec4 color, float rotation) const{
  this->Reg(pos, r, vertices, color, color, rotation);
}
void Renderer::Reg(hobot::Vec2 pos, float r, int vertices, hobot::Vec4 centerColor, hobot::Vec4 circumColor, float rotation) const{
  for (int i = 0; i < vertices; i++){
    float angle1 = ((float)i/(float)vertices)*2.0f*PI<float>() + (rotation);
    float angle2 = ((float)(i+1)/(float)vertices)*2.0f*PI<float>() + (rotation);
    hobot::Vec2 p1{pos.x+std::cos(angle1)*r,pos.y+std::sin(angle1)*r}, p2{pos.x+std::cos(angle2)*r,pos.y+std::sin(angle2)*r};

    hobot::Vec4 c1, c2;
    c1 = c2 = circumColor;

    this->Trig(pos, p1, p2, centerColor, c1, c2);
  }
}

void Renderer::FragShader(const char* string, bool isPath, Pipeline pipeline, bool recompile) const{
  switch (pipeline){
    case Pipeline::Fixed:
      _pImpl->fixedShader.Frag(string, isPath, recompile);
    break;
    case Pipeline::Raw:
      _pImpl->rawShader.Frag(string, isPath, recompile);
    break;
    case Pipeline::Sprite:
      _pImpl->spriteShader.Frag(string, isPath, recompile);
    break;
  }
}
void Renderer::VertShader(const char* string, bool isPath, Pipeline pipeline, bool recompile) const{
  switch (pipeline){
    case Pipeline::Fixed:
      _pImpl->fixedShader.Vert(string, isPath, recompile);
    break;
    case Pipeline::Raw:
      _pImpl->rawShader.Vert(string, isPath, recompile);
    break;
    case Pipeline::Sprite:
      _pImpl->spriteShader.Vert(string, isPath, recompile);
    break;
  }
}

//Use this if specifying both, otherwise errors are given as its trying to recompile with incompatible
void Renderer::Shaders(const char* vStr, const char* fStr, bool vIsPath, bool fIsPath, Pipeline pipeline)const{
  this->VertShader(vStr, vIsPath, pipeline, false);
  this->FragShader(fStr, fIsPath, pipeline, true);
}

const char* Renderer::DefaultFixedVertShader = 
"#version 330 core\n"
"layout (location = 0) in vec2 iPos;\n"
"layout (location = 1) in vec4 iColor;\n"
"out vec4 vColor;\n"
"void main(){\n"
"  gl_Position = vec4(iPos, 0, 1);\n"
"  vColor = iColor;\n"
"}\n";

const char* Renderer::DefaultFixedFragShader = 
"#version 330 core\n"
"layout (location = 0) out vec4 oColor;\n"
"in vec4 vColor;\n"
"void main(){\n"
"  oColor = vColor;\n"
"}\n";

const char* Renderer::DefaultSpriteFragShader =
"#version 330 core\n"
"layout (location = 0) out vec4 oColor;\n"
"in vec4 vColor;\n"
"in sampler2D vSprite;\n"
"in vec2 vTexCoord;\n"
"void main(){\n"
"  oColor = mix(vColor, texture(vSprite, vTexCoord), vec4(0.5, 0.5, 0.5, 0.5));\n"
"}\n";

const char* Renderer::DefaultSpriteVertShader =
"#version 330 core\n"
"layout (location = 0) in vec2 iPos;\n"
"layout (location = 1) in vec4 iColor;\n"
"layout (location = 2) in vec2 iTexCoord;\n"
"layout (location = 3) in sampler2D iSprite;\n"
"out vec4 vColor;\n"
"out sampler2D vSprite;\n"
"out vec2 vTexCoord;\n"
"void main(){\n"
"  gl_Position = vec4(iPos, 0, 1);\n"
"  vColor = iColor;\n"
"  vSprite = iSprite;\n"
"  vTexCoord = iTexCoord;\n"
"}\n";

#define UniformLogic() \
  switch (pipeline){\
    case Pipeline::Fixed:\
      _pImpl->fixedShader.SetUniform(name, v);\
    break;\
    case Pipeline::Raw:\
      _pImpl->rawShader.SetUniform(name, v);\
    break;\
    case Pipeline::Sprite:\
      _pImpl->spriteShader.SetUniform(name, v);\
    break;\
  }

void Renderer::Uniform(const char* name, int v,         Pipeline pipeline) const{
  UniformLogic();
}
void Renderer::Uniform(const char* name, float v,       Pipeline pipeline) const{
  UniformLogic();
}
void Renderer::Uniform(const char* name, hobot::Mat4 v, Pipeline pipeline) const{
  UniformLogic();
}
void Renderer::Uniform(const char* name, hobot::Vec4 v, Pipeline pipeline) const{
  UniformLogic();
}
void Renderer::Uniform(const char* name, hobot::Vec2 v, Pipeline pipeline) const{
  UniformLogic();
}

void Renderer::Clear(hobot::Vec4 color) const{
  this->_pImpl->clear = true;
  GLCall(glClearColor(color.x, color.y, color.z, color.w));
}

bool Renderer::IsValid() const{
  return _valid;
}

void Renderer::SetViewport(hobot::Vec2 start, hobot::Vec2 dimensions) const{
  _pImpl->viewport = {start, dimensions};

  //Set viewport
  glViewport(start.x*_props.width, start.y*_props.height, dimensions.x*_props.width, dimensions.y*_props.height);
  //How to display coordinates with respect to the window
  //left corner, right corner
  //Maps normalized device coordinates into window coordinates
  //Allows to make a literal viewport within the window and reserve other space for more (scales and maps)

  if (start == hobot::Vec2(0, 0) && dimensions == hobot::Vec2(1, 1)){
    glDisable(GL_SCISSOR_TEST); //Otherwise clear is too slow
  } else{
    glScissor(start.x*_props.width, start.y*_props.height, dimensions.x*_props.width, dimensions.y*_props.height); //Prevents blending into other viewports
    //Also clearing other viewports is prevented
    glEnable(GL_SCISSOR_TEST);
  }
}

void Renderer::_SetWindowProps(const WindowProps& props){
  _props = std::move(props);
}

hobot::Vec4 Renderer::GetViewport() const{
  return _pImpl->viewport;
}

//Custom pipeline
void Renderer::Raw(const void* data, unsigned int size, const std::vector<unsigned int>& indices) const{
  /*Raw IBO
  unsigned int& maxIndex = _pImpl->rawMaxIndex;
  unsigned int newMaxIndex = maxIndex;

  for (int i = 0; i < indices.size(); i++){
    newMaxIndex = std::max(newMaxIndex, indices[i]+maxIndex);
    _pImpl->rawIbo.emplace_back(indices[i]+maxIndex);
  }

  maxIndex = newMaxIndex+1;

  auto oldSize = _pImpl->rawSize;
  _pImpl->rawSize += size;
  if (_pImpl->rawSize > _pImpl->maxRawDataSize){
    _pImpl->pRawData = realloc(_pImpl->pRawData, _pImpl->rawSize);
    _pImpl->maxRawDataSize = _pImpl->rawSize;
  }
  std::memcpy(((char*)_pImpl->pRawData)+oldSize, data, size);
  */
}

void Renderer::RawLayout(const std::vector<LayoutElement>& layout) const{
  _pImpl->rawBuffer.Bind();

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, _pImpl->vboID));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pImpl->iboID));
  _pImpl->rawLayout.Reset();
  for (int i = 0; i < layout.size(); i++){
    _pImpl->rawLayout.Push(TypeToGLType(layout[i].type), layout[i].count, false);
  }

  _pImpl->rawBuffer.Unbind();
}

void Renderer::SetWireframe(bool enabled){
  if (enabled) {
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
  } else {
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
  }
}

void Renderer::Sprite(std::string path, hobot::Vec2 pos, hobot::Vec2 dimensions, hobot::Vec4 color) const{
  this->Sprite(path, pos, {pos.x+dimensions.x, pos.y}, {pos.x, pos.y+dimensions.y}, {pos.x+dimensions.x, pos.y+dimensions.y},
                     color, color, color, color,
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, false);
}
void Renderer::Sprite(std::string path, hobot::Vec2 pos0, hobot::Vec2 pos1, hobot::Vec2 pos2, hobot::Vec2 pos3,
                              hobot::Vec4 col0, hobot::Vec4 col1, hobot::Vec4 col2, hobot::Vec4 col3,
                              hobot::Vec2 tex0, hobot::Vec2 tex1, hobot::Vec2 tex2, hobot::Vec2 tex3, bool orderedMode) const{
  //Cache
  if (_pImpl->spriteTextureCache.contains(path)){
    _pImpl->spriteTextureCache[path].second = _pImpl->maxTextureSpriteCacheLifetime; //Update lifetime
  }else{
    _pImpl->spriteTextureCache.emplace(path, std::pair{std::make_shared<Texture>(path, true), _pImpl->maxTextureSpriteCacheLifetime});
  }

  auto& vbo = _pImpl->spriteVbo;
  auto& ibo = _pImpl->spriteIbo;
  int offset = vbo.size();
  int sampler = _pImpl->sprites.size();
  //Vbo
  vbo.emplace_back(SpriteVertex{pos0, col0, tex0, sampler});
  vbo.emplace_back(SpriteVertex{pos1, col1, tex1, sampler});
  vbo.emplace_back(SpriteVertex{pos2, col2, tex2, sampler});
  vbo.emplace_back(SpriteVertex{pos3, col3, tex3, sampler});

  //Push the sprite into sprites
  _pImpl->sprites.emplace_back(_pImpl->spriteTextureCache[path].first);

  //Ibo
  PushIboQuad();
}

}
