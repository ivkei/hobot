#pragma once

#include<string>
#include<sstream>
#include<fstream>
#include<filesystem>
#include<limits.h>

#include"ht_api.h"

namespace hobot{

//Gets dir of exec file
std::filesystem::path HOBOT_API GetExecDir();

//Takes in path relative to the exec's dir
std::string HOBOT_API ReadRel(std::string relPath);

class Image{
private:
  struct Impl;
  std::unique_ptr<Impl> _pImpl;
public:
  //Path is relative to executable
  Image(std::string relPath);
  ~Image();

  Image(Image&&);
  Image& operator=(Image&&);
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  void* RawData() const;
};

}
