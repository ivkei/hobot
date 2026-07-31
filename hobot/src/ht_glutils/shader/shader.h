#pragma once

#include<string>
#include<unordered_map>

#include"ht_math/math.h"

namespace hobot{

class Shader{
  private:
    unsigned int _id;
    unsigned int _vid;
    unsigned int _fid;
    std::unordered_map<std::string, int> _locationCache;

    int _GetUniformLocation(const std::string& name);
  public:
  Shader();
  ~Shader();

  void Frag(const char* string, bool isPath = false, bool recompile = true);
  void Vert(const char* string, bool isPath = false, bool recompile = true);
  void Bind() const;
  void Unbind() const;

  bool IsValid() const;

  //Uniforms
  void SetUniform(const std::string& name, const Mat4& mat);
  void SetUniform(const std::string& name, float f0, float f1, float f2, float f3);
  void SetUniform(const std::string& name, float f0);
  void SetUniform(const std::string& name, int i0);
  void SetUniform(const std::string& name, int is[], int count);
  void SetUniform(const std::string& name, hobot::Vec4 v);
  void SetUniform(const std::string& name, hobot::Vec2 v);
};

}
