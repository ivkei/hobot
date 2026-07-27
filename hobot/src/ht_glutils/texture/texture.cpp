#include"ht_pch/pch.h"

#include"texture.h"

#include"GL/glew.h"

#include"ht_glutils/debug/debug.h"

namespace hobot{

//TODO: add path support
Texture::Texture(std::string path){
}

Texture::Texture(Texture&& rhs)
: _width(rhs._width), _height(rhs._height), _id(rhs._id){
  rhs._id = 0;
}
Texture& Texture::operator=(Texture&& rhs){
  _width = rhs._width;
  _height = rhs._height;
  _id = rhs._id;
  rhs._id = 0;
  return *this;
}

Texture::Texture(unsigned int width, unsigned int height, bool isImage)
: _width(width), _height(height){
  GLCall(glGenTextures(1, &_id));
  GLCall(glBindTexture(GL_TEXTURE_2D, _id));

  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); //Done for each texture individually
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
  GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture(){
  if (_id != 0) GLCall(glDeleteTextures(1, &_id));
}

void Texture::Bind(unsigned int slot, bool image){
  if (image){
    //Bind texture to an image unit, allow to perform operations on it when passed as uniform
    GLCall(glBindImageTexture(slot, _id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
    //Slot, id, level, layered, layer, access, format
  }else{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, _id));
  }
}

void Texture::Unbind(){
  //Just nothing, it doesnt really matter
}

void Texture::Clear(glm::vec4 color){
  GLCall(glClearTexImage(_id, 0, GL_RGBA, GL_FLOAT, &color));
  //2nd is level
}

}
