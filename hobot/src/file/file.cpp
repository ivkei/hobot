#include"pch/pch.h"

#include"file.h"

namespace hobot{

#ifdef _HOBOT_WINDOWS
#include<windows.h>

std::filesystem::path GetExecDir(){
  //TODO: This could use some refactoring
  char* buffer = new char[512];
  DWORD len = GetModuleFileNameA(NULL, buffer, 512);
  if (len == 0){
    HT_LOG_ERROR("Couldnt get executable path!");
  }
  std::filesystem::path path = std::filesystem::path(buffer).parent_path();
  delete[] buffer;
  return path;
}
#elif _HOBOT_LINUX
#include<unistd.h>

std::filesystem::path GetExecDir(){
  char* buffer = new char[PATH_MAX];
  int len = readlink("/proc/self/exe", buffer, PATH_MAX);
  HT_LOG_ASSERT(len != -1, "Couldnt get executable path!");

  //Add null-terminator "\0"
  HT_LOG_ASSERT(len <= PATH_MAX, "Critical buffer overflow in GetExecDir()\tlen: ", len);
  buffer[len] = '\0';

  std::filesystem::path path = std::filesystem::path(buffer).parent_path();
  delete[] buffer;
  return path;
}
#else
#error "Unknown platform!"
#endif

//Takes in path relative to the exec's dir
std::string ReadRel(std::string relPath){
  std::filesystem::path fullPath = GetExecDir()/relPath;
  std::ifstream fs(fullPath.c_str());
  HT_LOG_ASSERT(fs.is_open(), "The shader location wasnt found; given: ", fullPath, "\tCWD: ", std::filesystem::current_path());
  std::stringstream res;
  res << fs.rdbuf();
  return res.str();
}


}
