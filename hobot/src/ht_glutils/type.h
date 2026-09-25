#pragma once

#include"ht_type.h"

#include"GL/glew.h"

namespace hobot{
  inline unsigned int TypeToGLType(Type type){
    switch (type){
      case Type::Float: return GL_FLOAT;
      case Type::Int: return GL_INT;
      case Type::UInt: return GL_UNSIGNED_INT;
      default: return GL_BYTE;
    };
  }

  inline int GLTypeToSize(unsigned int type){
    switch (type){
      case GL_FLOAT: return sizeof(float);
      case GL_INT: return sizeof(int);

      default:
      case GL_UNSIGNED_INT: return sizeof(unsigned int);
    };
  }

    template<class T>
    struct TToGLType;

    template<>
    struct TToGLType<int>{
      static constexpr unsigned int type = GL_INT;
    };
    template<>
    struct TToGLType<unsigned int>{
      static constexpr unsigned int type = GL_UNSIGNED_INT;
    };
    template<>
    struct TToGLType<float>{
      static constexpr unsigned int type = GL_FLOAT;
    };

}
