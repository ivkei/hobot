#pragma once

#include<string>
#include<vector>

#include"glm/glm.hpp"

namespace hobot{

class Texture{
private:
  unsigned int _id;
  unsigned int _width;
  unsigned int _height;
public:
  //Path to png image
  Texture(std::string path);

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(Texture&&);
  Texture& operator=(Texture&&);

  //Just texture
  //Image is same as default TEXTURE_2D but can also be written into in glsl and a little bit slower
  Texture(unsigned int width, unsigned int height, bool isImage = true);

  ~Texture();
  //Image is basically about being able to write at a little cost of speed (and image2D instead of sample2D have to be used)
  void Bind(unsigned int slot = 0, bool image = true);
  void Unbind(); //Unbinds from all slots

  inline unsigned int Width() const { return _width; }
  inline unsigned int Height() const { return _height; }

  void Clear(glm::vec4 color = glm::vec4(1));
};

}
