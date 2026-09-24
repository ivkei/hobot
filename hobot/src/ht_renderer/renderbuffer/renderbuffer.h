#pragma once

#include"ht_renderer/layoutelement.h"

#include<vector>
#include<memory>

namespace hobot{

class RenderBuffer{
private:
  struct Impl;
  std::unique_ptr<Impl> _pImpl;
public:
  enum Type{
    Static,
    Dynamic
  };

  RenderBuffer(Type type = Type::Static);
  ~RenderBuffer();
  
  //Size in bytes
  void Data(const void* pData, int size);
  void IndexData(const unsigned int* pData, int size);
  void SetLayout(const std::vector<LayoutElement>& layout);

  void Bind() const;
  void Unbind() const;
};

};
