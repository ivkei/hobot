#include"ht_pch/pch.h"

#include"ht_renderer/renderer.h"

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
  VAO fixedVao;
  //Raw
  void* pRawData = nullptr;
  unsigned int rawSize = 0;
  std::vector<unsigned int> rawIbo;
  unsigned int rawMaxIndex = 0; //1-indexed!
  Shader rawShader;
  VAO rawVao;
  VBOLayout rawLayout;
  unsigned int maxRawDataSize = 0;
  //Else
  unsigned int vboID = 0;
  unsigned int iboID = 0;
  bool clear = false;
  int maxVboSize = 0;
  int maxIboSize = 0;
  bool valid = true;
  hobot::Vec4 viewport;
  //Textures
  std::unordered_map<std::string, Texture> textures;
  int maxTextureSlots;
  int maxImageSlots;

  //Sprites
  std::unordered_map<std::string, std::pair<std::shared_ptr<Texture>, int>> spriteTextureCache; //cache textures with their lifetime counter by path
  int maxTextureSpriteCacheLifetime = 1; //How much renders it stays cached for, e.g. 1 implies that will get deleted on next render after
  Shader spriteShader;
  VAO spriteVao;
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
: _props(props){
  HT_LOG_INFO("Creating Renderer...");

  //Window initializes the context
  auto err = glewInit();
  auto valid = true;
  if (err != GLEW_OK){
    HT_LOG_ERROR("Failed to init glew: ", glewGetErrorString(err));
    valid = false;
    return;
  }

  //After glewInit
  _pImpl = std::make_unique<PImpl>();
  this->_pImpl->valid = valid;

  GLEnableAutoLogging();

  GLCall(HT_LOG_INFO("Initialized glew (version): ", glGetString(GL_VERSION)));

  //Blending
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glEnable(GL_BLEND));

  //VBO and IBO
  this->_pImpl->fixedVao.Bind();

  unsigned int& vboID = _pImpl->vboID;
  unsigned int& iboID = _pImpl->iboID;

  GLCall(glGenBuffers(1, &vboID));
  GLCall(glGenBuffers(1, &iboID));

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, vboID));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID));

  _pImpl->maxVboSize = 1000*sizeof(Vertex);
  _pImpl->maxIboSize = 1000*sizeof(unsigned int);
  GLCall(glBufferData(GL_ARRAY_BUFFER, _pImpl->maxVboSize, nullptr, GL_DYNAMIC_DRAW));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pImpl->maxIboSize, nullptr, GL_DYNAMIC_DRAW));

  //Fixed Layout
  VBOLayout layout;
  layout.Push<float>(2);
  layout.Push<float>(4);
  this->_pImpl->fixedVao.AddLayout(layout);

  HT_LOG_INFO("---Current default fixed vert shader---\n", this->DefaultFixedVertShader);
  HT_LOG_INFO("---Current default fixed frag shader---\n", this->DefaultFixedFragShader);

  //Fixed
  this->Shaders(DefaultFixedVertShader, DefaultFixedFragShader, false, false, Pipeline::Fixed);

  _pImpl->maxRawDataSize = 1000;
  _pImpl->pRawData = malloc(_pImpl->maxRawDataSize);

  //Textures
  GLCall(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_pImpl->maxTextureSlots));
  HT_LOG_INFO("Max texture slots: ", _pImpl->maxTextureSlots);

  GLCall(glGetIntegerv(GL_MAX_IMAGE_UNITS, &_pImpl->maxImageSlots));
  HT_LOG_INFO("Max image slots: ", _pImpl->maxImageSlots);

  //Sprite
  this->Shaders(DefaultSpriteVertShader, DefaultSpriteFragShader, false, false, Pipeline::Sprite);
  VBOLayout spriteLayout;
  spriteLayout.Push<float>(2); //Pos
  spriteLayout.Push<float>(4); //Color
  spriteLayout.Push<float>(2); //TexCoord
  spriteLayout.Push<int>(1); //Sampler
  _pImpl->spriteVao.AddLayout(spriteLayout);
}

Renderer::~Renderer(){
  if (this->IsValid()){
    GLCall(glDeleteBuffers(1, &this->_pImpl->vboID));
    GLCall(glDeleteBuffers(1, &this->_pImpl->iboID));
  }
  if (_pImpl->pRawData){
    free(_pImpl->pRawData);
  }
}

//Buffers data every frame, but otherwise too complex
void Renderer::Render() const{
  //Fixed
  const VAO& fixedVao = this->_pImpl->fixedVao;
  std::vector<Vertex>& fixedVbo = this->_pImpl->fixedVbo;
  std::vector<unsigned int>& fixedIbo = this->_pImpl->fixedIbo;
  const Shader& fixedShader = this->_pImpl->fixedShader;
  bool clear = this->_pImpl->clear;
  int maxVboSize = this->_pImpl->maxVboSize;
  int maxIboSize = this->_pImpl->maxIboSize;
  //Raw
  VAO& rawVao = _pImpl->rawVao;
  std::vector<unsigned int>& rawIbo = _pImpl->rawIbo;
  const Shader& rawShader = _pImpl->rawShader;
  void*& pRawData = _pImpl->pRawData;
  unsigned int& rawSize = _pImpl->rawSize;
  unsigned int& rawMaxIndex = _pImpl->rawMaxIndex;
  //Sprites
  auto& spriteVao = _pImpl->spriteVao;
  auto& spriteShader = _pImpl->spriteShader;
  auto& spriteIbo = _pImpl->spriteIbo;
  auto& spriteVbo = _pImpl->spriteVbo;
  auto& sprites = _pImpl->sprites;

  if (clear){
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
  }

  if (rawIbo.empty() && fixedIbo.empty() && spriteIbo.empty()) return;

  fixedVao.Bind();
  fixedShader.Bind();

  //Buffers (buffer everything here)
  //Single because (1) batching and (2) less binds
  //TODO: why 1 buffer, just do one for each pipeline
  if (maxVboSize >= (fixedVbo.size()*sizeof(Vertex) + rawSize)){
    //Dont reallocate data if not needed
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, fixedVbo.size()*sizeof(Vertex), fixedVbo.data()));
    if (rawSize > 0 && pRawData) GLCall(glBufferSubData(GL_ARRAY_BUFFER, fixedVbo.size()*sizeof(Vertex), rawSize, pRawData));
  }else{
    GLCall(glBufferData(GL_ARRAY_BUFFER, fixedVbo.size()*sizeof(Vertex)+rawSize, fixedVbo.data(), GL_DYNAMIC_DRAW));
    if (rawSize > 0 && pRawData) GLCall(glBufferSubData(GL_ARRAY_BUFFER, fixedVbo.size()*sizeof(Vertex), rawSize, pRawData));
  }
  if (maxIboSize >= (fixedIbo.size()+rawIbo.size())*sizeof(unsigned int)){
    GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, fixedIbo.size()*sizeof(unsigned int), fixedIbo.data()));
    if (rawIbo.size() > 2) GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, fixedIbo.size()*sizeof(unsigned int), rawIbo.size()*sizeof(unsigned int), rawIbo.data()));
  }else{
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (fixedIbo.size() + rawIbo.size())*sizeof(unsigned int), fixedIbo.data(), GL_DYNAMIC_DRAW));
    if (rawIbo.size() > 2) GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, fixedIbo.size()*sizeof(unsigned int), rawIbo.size()*sizeof(unsigned int), rawIbo.data()));
  }

  //Fixed pipeline rendering
  HT_LOG_ASSERT(fixedShader.IsValid(), "Fixed shader is invalid, please specify it with Vert and Frag");
  if (fixedIbo.size() > 2){
    glDrawElements(GL_TRIANGLES, fixedIbo.size(), GL_UNSIGNED_INT, NULL);
  }
  fixedVao.Unbind();
  fixedShader.Unbind();
  //Raw draw
  if (rawIbo.size() > 2 && pRawData){
    HT_LOG_ASSERT(rawShader.IsValid(), "Raw shader is invalid, please specify it with Vert and Frag");
    HT_LOG_ASSERT(rawVao.IsValid(), "Raw layout wasnt specified");
    rawShader.Bind();
    rawVao.Bind();
    _pImpl->rawLayout.SetOffset(fixedVbo.size()*sizeof(Vertex)); //offset within buffer
    rawVao.AddLayout(_pImpl->rawLayout);

    glDrawElements(GL_TRIANGLES, rawIbo.size(), GL_UNSIGNED_INT, (void*)(fixedIbo.size()*sizeof(unsigned int)));
    rawShader.Unbind();
    rawVao.Unbind();

    //Next batch preparation
    rawSize = 0;
    rawIbo.clear();
    rawMaxIndex = 0;
  }
  //Sprites (in batches of whatever number allowed)
  if (!spriteIbo.empty()){
    spriteShader.Bind();
    spriteVao.Bind();
    //TODO
  }

  //Prepare for the next batch
  this->_pImpl->clear = false;
  GLCall(glClearColor(0, 0, 0, 1));

  maxVboSize = std::max(maxVboSize, static_cast<int>(fixedVbo.size()*sizeof(Vertex) + rawSize));
  maxIboSize = std::max(maxIboSize, static_cast<int>((fixedIbo.size()+rawIbo.size())*sizeof(unsigned int)));

  fixedVbo.clear();
  fixedIbo.clear();

  //Sprites
  spriteIbo.clear();
  spriteVbo.clear();
  sprites.clear();
  for (auto&& i : _pImpl->spriteTextureCache){
    if (i.second.second-- <= 0) _pImpl->spriteTextureCache.erase(i.first);
  }
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
#define ifOrder(endpt1, endpt2, other1, other2)\
if ((pos##other1.y > AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y < AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2)) ||\
    (pos##other1.y < AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y > AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2))){\
  orderedIndices[0] = other1;\
  orderedIndices[1] = endpt1;\
  orderedIndices[2] = endpt2;\
  orderedIndices[3] = other2;\
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
  if (orderedMode){
    int orderedIndices[4] = {0, 1, 2, 3};
    ifOrder(0, 1, 2, 3)
    else ifOrder(0, 2, 1, 3)
    else ifOrder(1, 2, 0, 3)
    else ifOrder(0, 3, 1, 2)
    else ifOrder(1, 3, 0, 2)
    else ifOrder(3, 2, 0, 1);

    ibo.push_back(offset+orderedIndices[0]);
    ibo.push_back(offset+orderedIndices[1]);
    ibo.push_back(offset+orderedIndices[2]);
    ibo.push_back(offset+orderedIndices[1]);
    ibo.push_back(offset+orderedIndices[2]);
    ibo.push_back(offset+orderedIndices[3]);
  } else{
    ibo.push_back(offset+0);
    ibo.push_back(offset+1);
    ibo.push_back(offset+2);
    ibo.push_back(offset+1);
    ibo.push_back(offset+2);
    ibo.push_back(offset+3);
  }
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

const char* DefaultSpriteVertShader =
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
  return this->_pImpl->valid;
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
    glEnable(GL_SCISSOR_TEST);
  }
}

void Renderer::_SetWindowProps(WindowProps props){
  _props = std::move(props);
}

hobot::Vec4 Renderer::GetViewport() const{
  return _pImpl->viewport;
}

//Custom pipeline
void Renderer::Raw(const void* data, unsigned int size, const std::vector<unsigned int>& indices) const{
  //Raw IBO
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
}

static unsigned int HobotTypeToGLType(Type type){
  switch (type){
    case Type::Float: return GL_FLOAT;
    case Type::Int: return GL_INT;
    case Type::UInt: return GL_UNSIGNED_INT;
    default:
      HT_LOG_ERROR("Unknown type passed to HobotTypeToGLType");
      return GL_UNSIGNED_INT;
  }
}

void Renderer::RawLayout(const std::vector<LayoutElement>& layout) const{
  _pImpl->rawVao.Bind();
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, _pImpl->vboID));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pImpl->iboID));
  _pImpl->rawLayout.Reset();
  for (int i = 0; i < layout.size(); i++){
    _pImpl->rawLayout.Push(HobotTypeToGLType(layout[i].type), layout[i].count, false);
  }
  _pImpl->rawVao.Unbind();
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
  if (orderedMode){
    int orderedIndices[4] = {0, 1, 2, 3};
    ifOrder(0, 1, 2, 3)
    else ifOrder(0, 2, 1, 3)
    else ifOrder(1, 2, 0, 3)
    else ifOrder(0, 3, 1, 2)
    else ifOrder(1, 3, 0, 2)
    else ifOrder(3, 2, 0, 1);

    ibo.push_back(offset+orderedIndices[0]);
    ibo.push_back(offset+orderedIndices[1]);
    ibo.push_back(offset+orderedIndices[2]);
    ibo.push_back(offset+orderedIndices[1]);
    ibo.push_back(offset+orderedIndices[2]);
    ibo.push_back(offset+orderedIndices[3]);
  } else{
    ibo.push_back(offset+0);
    ibo.push_back(offset+1);
    ibo.push_back(offset+2);
    ibo.push_back(offset+1);
    ibo.push_back(offset+2);
    ibo.push_back(offset+3);
  }
}

}
