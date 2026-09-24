#pragma once

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"
#include"ht_logger.h"

#include"ht_renderer/layoutelement.h"

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
  struct TypeTo_GLType;

  template<>
  struct TypeTo_GLType<int>{
    static constexpr _GLType type = {GL_INT, sizeof(int)};
  };
  template<>
  struct TypeTo_GLType<float>{
    static constexpr _GLType type = {GL_FLOAT, sizeof(float)};
  };
  template<>
  struct TypeTo_GLType<unsigned int>{
    static constexpr _GLType type = {GL_UNSIGNED_INT, sizeof(unsigned int)};
  };
  template<>
  struct TypeTo_GLType<char>{
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
      _GLType type = TypeTo_GLType<T>::type;

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

    //LayoutElement version
    void Push(const LayoutElement& element){
      _GLType type = GLTypeTo_GLType(glType);
      //TODO: make
      //TODO: refactor this whole file with type.h glutil

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
