#include"renderbuffer.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/vao/vao.h"

namespace hobot{

struct RenderBuffer::Impl{
  unsigned int vid;
  unsigned int iid;

  unsigned int maxVboSize;
  unsigned int maxIboSize;

  VAO vao;
};

static unsigned int BufferTypeToGLType(RenderBuffer::Type type){
  switch (type){
    case RenderBuffer::Type::Static: return GL_STATIC_DRAW;
    case RenderBuffer::Type::Dynamic:
    default: return GL_DYNAMIC_DRAW;
  };
}

RenderBuffer::RenderBuffer(Type type)
: _pImpl(std::make_unique<Impl>(0,0,0,0)){
  this->Bind();
  
  GLCall(glGenBuffers(1, &_pImpl->iid));
  GLCall(glGenBuffers(1, &_pImpl->vid));

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, _pImpl->vid));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _pImpl->iid));

  _pImpl->maxVboSize = 1000;
  _pImpl->maxIboSize = 1000;
  GLCall(glBufferData(GL_ARRAY_BUFFER, _pImpl->maxVboSize, nullptr, BufferTypeToGLType(type)));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pImpl->maxIboSize, nullptr, BufferTypeToGLType(type)));

  this->Unbind();
}

RenderBuffer::~RenderBuffer(){
  this->Bind();

  GLCall(glDeleteBuffers(1, &_pImpl->vid));
  GLCall(glDeleteBuffers(1, &_pImpl->iid));

  this->Unbind();
}

//Size in bytes
void RenderBuffer::Data(const void* pData, int size){
  _pImpl->vao.Bind();
  //TODO
  _pImpl->vao.Unbind();
}

void RenderBuffer::IndexData(const unsigned int* pData, int size){
  _pImpl->vao.Bind();
  _pImpl->vao.Unbind();
}

void RenderBuffer::SetLayout(const std::vector<LayoutElement>& layout){
  _pImpl->vao.Bind();

  _pImpl->vao.AddLayout(layout);

  _pImpl->vao.Unbind();
}

void RenderBuffer::Bind() const{
  _pImpl->vao.Bind();
}

void RenderBuffer::Unbind() const{
  _pImpl->vao.Unbind();
}

}
