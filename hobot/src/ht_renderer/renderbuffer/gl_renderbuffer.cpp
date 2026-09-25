#include"renderbuffer.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/vao/vao.h"

namespace hobot{

struct RenderBuffer::Impl{
  unsigned int vid;
  unsigned int iid;

  unsigned int maxVboSize;
  unsigned int maxIboSize;

  unsigned int bufferType;

  VAO vao;
};

static unsigned int BufferTypeToGLType(RenderBuffer::BufferType type){
  switch (type){
    case RenderBuffer::BufferType::Static: return GL_STATIC_DRAW;
    case RenderBuffer::BufferType::Dynamic:
    default: return GL_DYNAMIC_DRAW;
  };
}

RenderBuffer::RenderBuffer(BufferType type)
: _pImpl(std::make_unique<Impl>(0,0,0,0, BufferTypeToGLType(type))){
  this->Bind();
  
  GLCall(glGenBuffers(1, &_pImpl->iid));
  GLCall(glGenBuffers(1, &_pImpl->vid));

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, _pImpl->vid));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pImpl->iid));

  _pImpl->maxVboSize = 1000;
  _pImpl->maxIboSize = 1000;
  GLCall(glBufferData(GL_ARRAY_BUFFER, _pImpl->maxVboSize, nullptr, _pImpl->bufferType));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pImpl->maxIboSize, nullptr, _pImpl->bufferType));

  this->Unbind();
}

RenderBuffer::~RenderBuffer(){
  this->Bind();

  GLCall(glDeleteBuffers(1, &_pImpl->vid));
  GLCall(glDeleteBuffers(1, &_pImpl->iid));

  this->Unbind();
}

static unsigned int DataTypeToGLType(RenderBuffer::DataType type){
  switch (type){
    case RenderBuffer::DataType::Vertex: return GL_ARRAY_BUFFER;
    case RenderBuffer::DataType::Index:
    default: return GL_ELEMENT_ARRAY_BUFFER;
  };
}

//Size in bytes
void RenderBuffer::Data(const void* pData, int size, RenderBuffer::DataType type){
  _pImpl->vao.Bind();

  unsigned int dataType = DataTypeToGLType(type);

  if (size > _pImpl->maxVboSize){
    GLCall(glBufferData(dataType, size, pData, _pImpl->bufferType));
    _pImpl->maxVboSize = size;
  }
  else{
    GLCall(glBufferSubData(dataType, 0, size, pData));
  }

  _pImpl->vao.Unbind();
}

void RenderBuffer::SetLayout(const std::vector<LayoutElement>& layout){
  _pImpl->vao.Bind();

  VBOLayout vboLayout;

  for (auto&& i : layout){
    vboLayout.Push(i);
  }

  _pImpl->vao.AddLayout(vboLayout);

  _pImpl->vao.Unbind();
}

void RenderBuffer::Bind() const{
  _pImpl->vao.Bind();
}

void RenderBuffer::Unbind() const{
  _pImpl->vao.Unbind();
}

}
