#include"renderbuffer.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/vao/vao.h"

#include<memory>
#include<string.h>

//DEBUG
#include<ht_renderer/renderable/renderable.h>

namespace hobot{

struct RenderBuffer::Impl{
  unsigned int vid;
  unsigned int iid;

  unsigned int maxVboSize;
  unsigned int maxIboSize;

  unsigned int bufferType;

  std::vector<char> vbo;
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

void RenderBuffer::Vertex(const void* pData, unsigned int size){
  _pImpl->vbo.insert(_pImpl->vbo.end(), (char*)pData, (char*)pData+size);
}
void RenderBuffer::Index(const std::vector<unsigned int>& indices, unsigned int offset){
  for (unsigned int i = 0; i < indices.size(); i++){
    _pImpl->ibo.push_back(indices[i]+offset);
  }
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
  _pImpl->vbo.clear();
  _pImpl->ibo.clear();
}

void RenderBuffer::Submit(){
  _pImpl->vao.Bind();

  //Vbo
  if (_pImpl->vbo.size() > _pImpl->maxVboSize){
    GLCall(glBufferData(GL_ARRAY_BUFFER, _pImpl->vbo.size()*2, nullptr, _pImpl->bufferType));
    _pImpl->maxVboSize = _pImpl->vbo.size()*2;
  }

  GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, _pImpl->vbo.size(), _pImpl->vbo.data()));

  //Ibo
  if (_pImpl->ibo.size() > _pImpl->maxIboSize){
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _pImpl->ibo.size()*2, nullptr, _pImpl->bufferType));
    _pImpl->maxIboSize = _pImpl->ibo.size()*2;
  }

  GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _pImpl->ibo.size(), _pImpl->ibo.data()));

  //DEBUG
  for (int i = 0; i < _pImpl->vbo.size()/sizeof(VertexData); i++){
    VertexData data = *(VertexData*)(_pImpl->vbo.data()+i*sizeof(VertexData));
    HT_LOG_INFO(data.pos, " ", data.col);
  }

  for (int i = 0; i < _pImpl->ibo.size(); i++){
    HT_LOG_INFO(_pImpl->ibo[i]);
  }
}

void RenderBuffer::Bind() const{
  _pImpl->vao.Bind();
}

void RenderBuffer::Unbind() const{
  _pImpl->vao.Unbind();
}

unsigned int RenderBuffer::VertexSize() const{
  return _pImpl->vbo.size();
}

unsigned int RenderBuffer::IndexSize() const{
  return _pImpl->ibo.size()*sizeof(unsigned int);
}

}
