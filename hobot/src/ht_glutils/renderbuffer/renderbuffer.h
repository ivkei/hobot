#pragma once

#include"ht_glutils/shader/shader.h"
#include"ht_glutils/vbolayout.h"
#include"ht_glutils/vao/vao.h"

namespace hobot{

class RenderBuffer{
private:
  VAO _vao;
  unsigned int _iid;
  unsigned int _vid;
public:
  RenderBuffer();
  ~RenderBuffer();
  
  void Render(const Shader& shader);

  //Size in bytes
  void Data(const void* pData, int size);

  void IndexData(const unsigned int* pData, int size);

  void SetLayout(const VBOLayout& layout);
};

};
