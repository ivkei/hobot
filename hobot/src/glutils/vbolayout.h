#pragma once

#include"GL/glew.h"

#include"glutils/debug/debug.h"
#include"logger.h"

#include<vector>

namespace hobot{
  struct VBOLayoutElement{
    unsigned int count;
    unsigned int size;
    unsigned int type;
    bool normalize;
  };

  struct _GLType{
    unsigned int type;
    unsigned int size;
  };

  template<class T>
  struct TypeToGLType;

  template<>
  struct TypeToGLType<int>{
    static constexpr _GLType type = {GL_INT, sizeof(int)};
  };
  template<>
  struct TypeToGLType<float>{
    static constexpr _GLType type = {GL_FLOAT, sizeof(float)};
  };
  template<>
  struct TypeToGLType<unsigned int>{
    static constexpr _GLType type = {GL_UNSIGNED_INT, sizeof(unsigned int)};
  };
  template<>
  struct TypeToGLType<char>{
    static constexpr _GLType type = {GL_BYTE, sizeof(char)};
  };

  class VBOLayout final{
  private:
    std::vector<VBOLayoutElement> _elems;
    unsigned int _stride;
  public:
    VBOLayout() : _stride(0) {}
    ~VBOLayout() = default;

    template<class T>
    void Push(unsigned int count, bool normalized = false){
      _GLType type = TypeToGLType<T>::type;

      unsigned int size = type.size*count;
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{count, size, type.type, normalized});
    }

    inline const std::vector<VBOLayoutElement>& GetElements() const { return _elems; }
    inline unsigned int GetStride() const { return _stride; }
  };

}
