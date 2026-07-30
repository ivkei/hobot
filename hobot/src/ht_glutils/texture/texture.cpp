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

  //Texture coordinates dont depend on resolution but can be any floating point value.
  //Thus, OpenGL has to figure out which texture pixel (texel) to map the texture coordinate to.
  //Think of a very large object with a low resolution texture upon it
  //Called texture filtering options
  //GL_NEAREST (aka nearest neighbor or point filtering) is default, select texel with closest center
  //  - Pixelated (upscale)
  //GL_LINEAR (aka Bilinear filtering) interpolates based on position (weighted interpolation)
  //  - Blurred (upscale)
  //Magnifying/Minifying operations
  //For minifying, if the aspect ratio is different, where each pixel doesnt map to a texel, the texture filtering is needed
  //If theres a pattern such as a brick wall, just choosing with intervals wont display it potentially
  //And flickering will occur as even the slightest change will change the texel sampled
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); //Done for each texture individually
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); //S (X)
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); //T (Y)
                                                                               //R (Z)
  //If specify coordinates outside 0-1 box (Texture wrapping)
  //Options:
  //  - GL_REPEAT (default)
  //  - GL_MIRRORED_REPEAT
  //  - GL_CLAMP_TO_EDGE
  //  - GL_CLAMP_TO_BORDER (requires border color specified via glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, <borderColor>))

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
