#pragma once

namespace hobot{

//Note that glew has to be init for this
void GLEnableAutoLogging();
void GLClearErrors();
void GLLogErrors(const char* command, const char* file, int line);

#define GLCall(x)\
  GLClearErrors();\
  x;\
  GLLogErrors(#x, __FILE__, __LINE__)

}
