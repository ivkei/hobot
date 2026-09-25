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
  enum BufferType{
    Static,
    Dynamic
  };
  enum DataType{
    Vertex,
    Index
  };

  RenderBuffer(BufferType type = BufferType::Static);
  ~RenderBuffer();
  
  //Size in bytes
  //Replaces old data
  void Data(const void* pData, int size, DataType type);
  void SetLayout(const std::vector<LayoutElement>& layout);

  void Bind() const;
  void Unbind() const;
};

};
