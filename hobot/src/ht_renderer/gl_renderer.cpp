#include"ht_pch/pch.h"

#include"ht_renderer/renderer.h"

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/shader/shader.h"
#include"ht_glutils/vao/vao.h"

#define PI 3.141592653589793115997963468544185161590576171875

namespace hobot{

struct Vertex{
  glm::vec2 pos;
  glm::vec4 color;
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
  int rawMaxIndex = 0; //TODO: finish  this, make it 1-indexed
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
  glm::vec4 viewport;
};

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


  //Layout
  VBOLayout layout;
  layout.Push<float>(2);
  layout.Push<float>(4);
  this->_pImpl->fixedVao.AddLayout(layout);

  HT_LOG_INFO("---Current default vert shader---\n", this->DefaultVertShader);
  HT_LOG_INFO("---Current default frag shader---\n", this->DefaultFragShader);

  //Set default fixed ones
  this->FragShader();
  this->VertShader();

  _pImpl->maxRawDataSize = 1000;
  _pImpl->pRawData = malloc(_pImpl->maxRawDataSize);
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
  int& rawLastIndex = _pImpl->rawLastIndex;

  if (clear){
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
  }

  fixedVao.Bind();
  fixedShader.Bind();

  //Buffers (buffer both raw and fixed info)
  //Single because (1) batching and (2) less binds
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
    rawShader.Bind();
    rawVao.Bind();
    _pImpl->rawLayout.SetOffset(fixedVbo.size()*sizeof(Vertex)); //offset within buffer
    rawVao.AddLayout(_pImpl->rawLayout);

    HT_LOG_INFO("Offset: ", fixedVbo.size()*sizeof(Vertex));
    HT_LOG_INFO("Vertices (Fixed): ", fixedVbo.size());
    HT_LOG_INFO("Vertices (Raw): ", rawSize/(sizeof(float)*6));
    HT_LOG_INFO("Indices (Raw): ", rawIbo.size());

    glDrawElements(GL_TRIANGLES, rawIbo.size(), GL_UNSIGNED_INT, NULL);

    rawShader.Unbind();
    rawVao.Unbind();

    //Next batch preparation
    rawSize = 0;
    rawIbo.clear();
    rawLastIndex = -1;
  }

  //Prepare for the next batch
  this->_pImpl->clear = false;
  GLCall(glClearColor(0, 0, 0, 1));

  maxVboSize = std::max(maxVboSize, static_cast<int>(fixedVbo.size()*sizeof(Vertex) + rawSize));
  maxIboSize = std::max(maxIboSize, static_cast<int>((fixedIbo.size()+rawIbo.size())*sizeof(unsigned int)));

  fixedVbo.clear();
  fixedIbo.clear();
}

static float AtFor2Pts(float x, glm::vec2 p1, glm::vec2 p2){
  return (p1.x != p2.x) && (p1.x*p2.x - p1.y*p2.y-x*(p2.y-p1.y))/(p1.x-p2.x);
}

//If 2 points divide the remaining 2 points so that they are on opposite sides, those 2 endpts can be used to draw 2 triangles no matter what order
//This orders the indices to disregard invalid order of input
#define ifOrder(endpt1, endpt2, other1, other2)\
if ((pos##other1.y > AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y < AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2)) ||\
    (pos##other1.y < AtFor2Pts(pos##other1.x, pos##endpt1, pos##endpt2) && pos##other2.y > AtFor2Pts(pos##other2.x, pos##endpt1, pos##endpt2))){\
  orderedIndices[0] = other1;\
  orderedIndices[1] = endpt1;\
  orderedIndices[2] = endpt2;\
  orderedIndices[3] = other2;\
}

static void PushQuad(std::vector<Vertex>& vao, std::vector<unsigned int>& ibo, glm::vec2 pos0, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, //Dont rename them
    glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, glm::vec4 c4, bool orderedMode){
  int offset = vao.size();
  //Vbo
  vao.emplace_back(Vertex{pos0, c1});
  vao.emplace_back(Vertex{pos1, c2});
  vao.emplace_back(Vertex{pos2, c3});
  vao.emplace_back(Vertex{pos3, c4});

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

static void PushTrig(std::vector<Vertex>& vao, std::vector<unsigned int>& ibo, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3,
    glm::vec4 c1, glm::vec4 c2, glm::vec4 c3){
  int offset = vao.size();
  //Vbo
  vao.emplace_back(Vertex{pos1, c1});
  vao.emplace_back(Vertex{pos2, c2});
  vao.emplace_back(Vertex{pos3, c3});

  //Ibo
  ibo.push_back(offset);
  ibo.push_back(offset+1);
  ibo.push_back(offset+2);
}

static glm::vec4 RadsToColor(float rads){
  if (0 <= rads && rads < (2*PI)/3){
    auto interpol = rads/((2*PI)/3);
    return {interpol, 1-interpol, 0, 1};
  } else if ((2*PI)/3 <= rads && rads < (4*PI)/3){
    auto interpol = (rads-((2*PI)/3))/((2*PI)/3);
    return {1-interpol, 0, interpol, 1};
  } else{
    auto interpol = (rads-((4*PI)/3))/((2*PI)/3);
    return {0, interpol, 1-interpol, 1};
  }
}

static void PushCircle(std::vector<Vertex>& vao, std::vector<unsigned int>& ibo, glm::vec2 center, float r, int iters, glm::vec4 centerColor, glm::vec4 circumColor, bool rainbowCircum){
  for (int i = 0; i < iters; i++){
    float angle1 = ((float)i/(float)iters)*2.0f*PI;
    float angle2 = ((float)(i+1)/(float)iters)*2.0f*PI;
    glm::vec2 p1{center.x+std::cos(angle1)*r,center.y+std::sin(angle1)*r}, p2{center.x+std::cos(angle2)*r,center.y+std::sin(angle2)*r};

    glm::vec4 c1, c2;
    if (!rainbowCircum){
      c1 = c2 = circumColor;
    }else{
      c1 = RadsToColor(angle1);
      c2 = RadsToColor(angle2);
    }

    PushTrig(vao, ibo, center, p1, p2, centerColor, c1, c2);
  }
}

//pos = bottom-left vertex pos, dimensions = width, height
void Renderer::Quad(glm::vec2 pos, glm::vec2 dimensions, glm::vec4 color) const{
  PushQuad(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos, {pos.x+dimensions.x, pos.y}, {pos.x, pos.y+dimensions.y}, {pos.x+dimensions.x, pos.y+dimensions.y},
      color, color, color, color, false);
}
void Renderer::Quad(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4,
          glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, glm::vec4 c4, bool orderedMode) const{
  PushQuad(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos1, pos2, pos3, pos4, c1, c2, c3, c4, orderedMode);
}

//pos = bottom-left vertex pos, dimensions = base width, height, triangle = right
void Renderer::Trig(glm::vec2 pos, glm::vec2 dimensions, glm::vec4 color) const{
  PushTrig(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos, {pos.x+dimensions.x, pos.y}, {pos.x, pos.y + dimensions.y}, color, color, color);
}
void Renderer::Trig(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3,
          glm::vec4 c1, glm::vec4 c2, glm::vec4 c3) const{
  PushTrig(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos1, pos2, pos3, c1, c2, c3);
}

//pos = center of the circle coordinates
void Renderer::Circle(glm::vec2 pos, float r, int vertices, glm::vec4 color) const{
  PushCircle(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos, r, vertices, color, color, false);
}
//pos = center of the circle coordinates
void Renderer::Circle(glm::vec2 pos, float r, int vertices, glm::vec4 centerColor, glm::vec4 circumColor) const{
  PushCircle(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos, r, vertices, centerColor, circumColor, false);
}
//pos = center of the circle coordinates
void Renderer::Circle(glm::vec2 pos, float r, int vertices, bool isRainbow) const{
  PushCircle(this->_pImpl->fixedVbo, this->_pImpl->fixedIbo, pos, r, vertices, glm::vec4(1, 1, 1, 1), glm::vec4(0), true);
}

void Renderer::FragShader(const char* string, bool isPath, bool fixed, bool recompile) const{
  if (fixed){
    _pImpl->fixedShader.Frag(string, isPath, recompile);
  }else{
    _pImpl->rawShader.Frag(string, isPath, recompile);
  }
}
void Renderer::VertShader(const char* string, bool isPath, bool fixed, bool recompile) const{
  if (fixed){
    _pImpl->fixedShader.Vert(string, isPath, recompile);
  }else{
    _pImpl->rawShader.Vert(string, isPath, recompile);
  }
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

void Renderer::Uniform(const char* name, int i, bool fixed) const{
  if (fixed){
    _pImpl->fixedShader.Bind();
    _pImpl->fixedShader.SetUniform(name, i);
  }else{
    _pImpl->rawShader.Bind();
    _pImpl->rawShader.SetUniform(name, i);
  }
}
void Renderer::Uniform(const char* name, float f, bool fixed) const{
  if (fixed){
    _pImpl->fixedShader.Bind();
    _pImpl->fixedShader.SetUniform(name, f);
  }else{
    _pImpl->rawShader.Bind();
    _pImpl->rawShader.SetUniform(name, f);
  }
}
void Renderer::Uniform(const char* name, glm::mat4 m, bool fixed) const{
  if (fixed){
    _pImpl->fixedShader.Bind();
    _pImpl->fixedShader.SetUniform(name, m);
  }else{
    _pImpl->rawShader.Bind();
    _pImpl->rawShader.SetUniform(name, m);
  }
}

void Renderer::Clear(glm::vec4 color) const{
  this->_pImpl->clear = true;
  GLCall(glClearColor(color.x, color.y, color.z, color.w));
}

bool Renderer::IsValid() const{
  return this->_pImpl->valid;
}

void Renderer::SetViewport(glm::vec2 start, glm::vec2 dimensions) const{
  _pImpl->viewport = {start, dimensions};

  //Set viewport
  glViewport(start.x*_props.width, start.y*_props.height, dimensions.x*_props.width, dimensions.y*_props.height);
  //How to display coordinates with respect to the window
  //left corner, right corner
  //Maps normalized device coordinates into window coordinates
  //Allows to make a literal viewport within the window and reserve other space for more (scales and maps)

  if (start == glm::vec2(0, 0) && dimensions == glm::vec2(1, 1)){
    glDisable(GL_SCISSOR_TEST); //Otherwise clear is too slow
  } else{
    glScissor(start.x*_props.width, start.y*_props.height, dimensions.x*_props.width, dimensions.y*_props.height); //Prevents blending into other viewports
    glEnable(GL_SCISSOR_TEST);
  }
}

void Renderer::_SetWindowProps(WindowProps props){
  _props = std::move(props);
}

glm::vec4 Renderer::GetViewport() const{
  return _pImpl->viewport;
}

//Custom pipeline
void Renderer::Raw(const void* data, unsigned int size, const std::vector<unsigned int>& indices) const{
  //Raw IBO
  int& lastIndex = _pImpl->rawLastIndex;
  int newLastIndex = lastIndex;

  for (int i = 0; i < indices.size(); i++){
    HT_LOG_INFO(indices[i]+lastIndex);
    newLastIndex = std::max(newLastIndex, (int)indices[i]+lastIndex);
    _pImpl->rawIbo.emplace_back(indices[i]+lastIndex);
  }

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

}
