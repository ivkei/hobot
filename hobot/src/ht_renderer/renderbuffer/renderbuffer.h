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
    Static, //Correspond to GL_STATIC_DRAW
    Dynamic //Ditto
  };

  RenderBuffer(BufferType type = BufferType::Static);
  ~RenderBuffer();

  //Size in bytes
  //Accumulates the data (Clear() and Submit() may be of help)
  //Note that binds and doesnt unbind, since would like to prevent unexpected unbinding
  void Vertex(const void* pData, unsigned int size);
  void Index(const std::vector<unsigned int>& indices);
  void SetLayout(const std::vector<LayoutElement>& layout, unsigned int offset = 0);

  void Clear(); //Clear the accumulated data
  void Submit(); //Submit the accumulated data to the GPU (batching)

  void Bind() const;
  void Unbind() const;
};

};
