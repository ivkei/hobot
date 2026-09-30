#include"renderbuffer.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/vao/vao.h"

#include<memory>
#include<string.h>

namespace hobot{

struct RenderBuffer::Impl{
  unsigned int vid;
  unsigned int iid;

  unsigned int maxVboSize;
  unsigned int maxIboSize;

  unsigned int bufferType;

  unsigned int offset; //In bytes

  void* pData;
  unsigned int size; //In bytes

  std::vector<unsigned int> ibo;

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
: _pImpl(std::make_unique<Impl>(0,0,0,0, BufferTypeToGLType(type), 0, nullptr, 0)){
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

  _pImpl->pData = malloc(_pImpl->maxVboSize);
}

RenderBuffer::~RenderBuffer(){
  this->Bind();

  GLCall(glDeleteBuffers(1, &_pImpl->vid));
  GLCall(glDeleteBuffers(1, &_pImpl->iid));

  this->Unbind();

  free(_pImpl->pData);
}

void RenderBuffer::Vertex(const void* pData, unsigned int size){
  _pImpl->pData = realloc(_pImpl->pData, _pImpl->offset+size);
  memcpy((char*)_pImpl->pData+_pImpl->offset, pData, size);
}
void RenderBuffer::Index(const std::vector<unsigned int>& indices){
  _pImpl->ibo.insert(_pImpl->ibo.end(), indices.begin(), indices.end());
}

void RenderBuffer::SetLayout(const std::vector<LayoutElement>& layout, unsigned int offset){
  _pImpl->vao.Bind();

  VBOLayout vboLayout;

  for (auto&& i : layout){
    vboLayout.Push(i);
  }

  vboLayout.SetOffset(offset);

  _pImpl->vao.AddLayout(vboLayout);
}

void RenderBuffer::Clear(){
  //TODO: right?
  _pImpl->offset = 0;
  _pImpl->size = 0;
}

void RenderBuffer::Submit(){
  _pImpl->vao.Bind();\
  if (size > _pImpl->max){\
    GLCall(glBufferData(type, size, pData, _pImpl->bufferType));\
    _pImpl->max = size;\
  }\
  else{\
    GLCall(glBufferSubData(type, 0, size, pData));\
  }\
  //TODO
}

void RenderBuffer::Bind() const{
  _pImpl->vao.Bind();
}

void RenderBuffer::Unbind() const{
  _pImpl->vao.Unbind();
}

}
