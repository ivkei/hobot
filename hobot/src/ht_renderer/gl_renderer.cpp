#include"ht_pch/pch.h"

#include"ht_renderer/renderer.h"
#include"ht_renderer/renderbuffer/renderbuffer.h"

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/shader/shader.h"
#include"ht_glutils/vao/vao.h"
#include"ht_glutils/texture/texture.h"

namespace hobot{

struct Renderer::PImpl{
  Shader shader;
  RenderBuffer renderBuffer{RenderBuffer::BufferType::Dynamic};

  unsigned int indices = 0;

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
  //std::vector<SpriteVertex> spriteVbo;
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
  _pImpl->renderBuffer.SetLayout(VertexData::layout);

  //Shaders
  HT_LOG_INFO("---Current default fixed vert shader---\n", this->DefaultVertShader);
  HT_LOG_INFO("---Current default fixed frag shader---\n", this->DefaultFragShader);

  //Fixed
  this->Shaders(DefaultVertShader, DefaultFragShader, false, false);

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
}

//Buffers data every frame, but otherwise too complex
void Renderer::Render() const{
  //Clear
  if (_pImpl->clear){
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
  }
  _pImpl->clear = false;
  GLCall(glClearColor(0, 0, 0, 1));

  //Render
  if (_pImpl->indices == 0) return;

  _pImpl->shader.Bind();
  _pImpl->renderBuffer.Bind();
  GLCall(glDrawElements(GL_TRIANGLES, _pImpl->indices, GL_UNSIGNED_INT, 0));

  //Next batch
  _pImpl->renderBuffer.Clear();
  _pImpl->indices = 0;
}

void Renderer::Submit(const std::vector<VertexData>& vertex, const std::vector<unsigned int>& index){
  _pImpl->renderBuffer.Vertex(vertex.data(), vertex.size()*sizeof(VertexData));
  _pImpl->renderBuffer.Index(index, _pImpl->renderBuffer.VertexSize());

  _pImpl->indices += index.size();
}

void Renderer::FragShader(const char* string, bool isPath, bool recompile) const{
  _pImpl->shader.Frag(string, isPath, recompile);
}

void Renderer::VertShader(const char* string, bool isPath, bool recompile) const{
  _pImpl->shader.Vert(string, isPath, recompile);
}

//Use this if specifying both, otherwise errors are given as its trying to recompile with incompatible
void Renderer::Shaders(const char* vStr, const char* fStr, bool vIsPath, bool fIsPath)const{
  this->VertShader(vStr, vIsPath, false);
  this->FragShader(fStr, fIsPath, true);
}

const char* Renderer::DefaultVertShader = 
"#version 330 core\n"
"layout (location = 0) in vec2 iPos;\n"
"layout (location = 1) in vec4 iColor;\n"
"out vec4 vColor;\n"
"void main(){\n"
"  gl_Position = vec4(iPos, 0, 1);\n"
"  vColor = iColor;\n"
"}\n";

const char* Renderer::DefaultFragShader = 
"#version 330 core\n"
"layout (location = 0) out vec4 oColor;\n"
"in vec4 vColor;\n"
"void main(){\n"
"  oColor = vColor;\n"
"}\n";

/*
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
*/

void Renderer::Uniform(const char* name, int v) const{
  _pImpl->shader.SetUniform(name, v);\
}
void Renderer::Uniform(const char* name, float v) const{
  _pImpl->shader.SetUniform(name, v);\
}
void Renderer::Uniform(const char* name, hobot::Mat4 v) const{
  _pImpl->shader.SetUniform(name, v);\
}
void Renderer::Uniform(const char* name, hobot::Vec4 v) const{
  _pImpl->shader.SetUniform(name, v);\
}
void Renderer::Uniform(const char* name, hobot::Vec2 v) const{
  _pImpl->shader.SetUniform(name, v);\
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

void Renderer::SetWireframe(bool enabled){
  if (enabled) {
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
  } else {
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
  }
}

/*
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
*/

}
