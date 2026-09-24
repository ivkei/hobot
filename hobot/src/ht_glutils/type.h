#pragma once

#include"ht_type.h"

#include"GL/glew.h"

namespace hobot{
  //TODO: refactor all this
  inline unsigned int TypeToGLType(Type type){
    switch (type){
      case Type::Float: return GL_FLOAT;
      case Type::Int: return GL_INT;
      case Type::UInt: return GL_UNSIGNED_INT;
      default: return GL_BYTE;
    };
  }

  inline int GLTypeToSize(unsigned int type){
    //TODO
  }
};
