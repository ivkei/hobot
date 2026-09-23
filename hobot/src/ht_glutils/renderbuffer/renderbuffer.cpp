#include"renderbuffer.h"

#include"ht_glutils/debug/debug.h"

namespace hobot{

RenderBuffer::RenderBuffer()
: _vao{}, _iid{0}, _vid{0}{
  _vao.Bind();

  GLCall(glGenBuffers(1, &_iid));
  GLCall(glGenBuffers(1, &_vid));

  //TODO: implement this and replace renderer's logic with this

  _vao.Unbind();
}

RenderBuffer::~RenderBuffer(){

}

void RenderBuffer::Render(const Shader& shader){
}

//Size in bytes
void RenderBuffer::Data(const void* pData, int size){
}

void RenderBuffer::IndexData(const unsigned int* pData, int size){
}

void RenderBuffer::SetLayout(const VBOLayout& layout){
}

}
