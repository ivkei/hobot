#pragma once

#include"glutils/vbolayout.h"

namespace hobot{

//Vertex array object
//Its main goal is to store the attribute layout
class VAO{
private:
  unsigned int _id;
public:
  VAO();
  ~VAO();

  void AddLayout(const VBOLayout& layout) const;

  void Bind() const;
  void Unbind() const;
};

}
