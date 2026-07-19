#pragma once

#include<string>

namespace hobot{

struct WindowProps{
  int width;
  int height;
  std::string name;
  float apiVersion;
  bool resizable;
};

}
