#pragma once

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"
#include"ht_glutils/type.h"
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
      unsigned int glType = TToGLType<T>::type;

      unsigned int size = GLTypeToSize(glType)*count;
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{count, size, glType, normalized});
    }

    //Non-template version
    void Push(unsigned int glType, unsigned int count, bool normalized = false){

      unsigned int size = GLTypeToSize(glType)*count;
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{count, size, glType, normalized});
    }

    //LayoutElement version
    void Push(const LayoutElement& element){
      unsigned int glType = TypeToGLType(element.type);

      unsigned int size = element.count*GLTypeToSize(glType);
      _stride+=size;
      _elems.emplace_back(VBOLayoutElement{element.count, size, glType, false});
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
