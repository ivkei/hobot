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

  RenderBuffer(BufferType type = BufferType::Static);
  ~RenderBuffer();
  
  //Size in bytes
  //Replaces old data
  //Note that binds and doesnt unbind, since would like to prevent unexpected unbinding
  void Vertex(const void* pData, unsigned int size);
  void Index(const void* pData, unsigned int size);
  void SetLayout(const std::vector<LayoutElement>& layout, unsigned int offset = 0);

  void Bind() const;
  void Unbind() const;
};

};
