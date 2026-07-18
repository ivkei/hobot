#include"pch/pch.h"

#include"vao.h"

#include"glutils/debug/debug.h"

#include"GL/glew.h"

namespace hobot{

VAO::VAO(){
  GLCall(glGenVertexArrays(1, &_id));
}

VAO::~VAO(){
  GLCall(glDeleteVertexArrays(1, &_id));
}

void VAO::AddLayout(const VBOLayout& layout) const{
  this->Bind();

  const auto& elems = layout.GetElements();
  unsigned int stride = layout.GetStride();
  long long offset = 0;

  for (int i = 0; i < elems.size(); i++){
    const auto& elem = elems[i];
    GLCall(glVertexAttribPointer(i, elem.count, elem.type, elem.normalize, stride, (const void*)offset));
    GLCall(glEnableVertexAttribArray(i));
    offset+=elem.size;
  }
}

void VAO::Bind() const{
  GLCall(glBindVertexArray(_id));
}

void VAO::Unbind() const{
  GLCall(glBindVertexArray(0));
}

}
