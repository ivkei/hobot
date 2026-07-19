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

  static _GLType GLTypeTo_GLType(unsigned int type){
    switch (type){
      case GL_FLOAT: return {GL_FLOAT, sizeof(float)};
      case GL_INT: return {GL_INT, sizeof(int)};
      case GL_UNSIGNED_INT: return {GL_UNSIGNED_INT, sizeof(unsigned int)};
      case GL_BYTE: return {GL_BYTE, sizeof(char)};
      default: 
        HT_LOG_ERROR("Unknown type passed to GLTypeTo_GLType");
        return {GL_UNSIGNED_INT, sizeof(unsigned int)};
    }
  }

  class VBOLayout final{
  private:
    std::vector<VBOLayoutElement> _elems;
    unsigned int _stride = 0;
    unsigned int _offset = 0;
  public:
    VBOLayout() = default;
    ~VBOLayout() = default;

    template<class T>
    void Push(unsigned int count, bool normalized = false){
      _GLType type = TypeToGLType<T>::type;

      unsigned int size = type.size*count;
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{count, size, type.type, normalized});
    }

    //Non-template version
    void Push(unsigned int glType, unsigned int count, bool normalized = false){
      _GLType type = GLTypeTo_GLType(glType);

      unsigned int size = type.size*count;
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{count, size, type.type, normalized});
    }

    //0 by default
    inline void SetOffset(unsigned int offset) { _offset = offset; }

    inline const std::vector<VBOLayoutElement>& GetElements() const { return _elems; }
    inline unsigned int GetStride() const { return _stride; }
    inline unsigned int GetOffset() const { return _offset; }

    void Reset(){
      _stride = 0;
      _offset = 0;
      _elems.clear();
    }
  };

}
