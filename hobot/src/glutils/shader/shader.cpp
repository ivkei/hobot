#include"pch/pch.h"

#include"shader.h"

#include"glutils/debug/debug.h"

#include"GL/glew.h"

namespace hobot{

static unsigned int CreateShader(unsigned int type, std::string src){
  GLCall(unsigned int shaderObj = glCreateShader(type));
  const char* cSrc = src.c_str();

  //Attach the source code
  GLCall(glShaderSource(shaderObj, 1, &cSrc, NULL));
  //ID, # of strings as source code, length of each string passed (in case not C-style termination, "null terminator")

  //Compile the shader
  GLCall(glCompileShader(shaderObj));

  //Check successful compilation
  int success;
  GLCall(glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success)); //success has to be a vector of ints (for function), here, though, 1 int is suffient
  if (!success){
    char infoLog[512];
    GLCall(glGetShaderInfoLog(shaderObj, 512, NULL, infoLog)); //3rd is length of string of infoLog
    HT_LOG_ERROR("Shader failed to compile: ", infoLog);
  }

  return shaderObj;
}

static void LinkProgram(unsigned int shaderProgram, unsigned int vso, unsigned int fso){
  GLCall(glAttachShader(shaderProgram, vso));
  GLCall(glAttachShader(shaderProgram, fso));
  GLCall(glLinkProgram(shaderProgram));

  //Check failure
  int success;
  GLCall(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success));
  if (!success){
    char infoLog[512];
    GLCall(glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog));
    HT_LOG_ERROR("Shader linking error: ", infoLog);
  }

  //Detach shader so that the linker doesnt use them anymore
  GLCall(glDetachShader(shaderProgram, vso));
  GLCall(glDetachShader(shaderProgram, fso));

  //Delete the shader objects
  //Rather marks them for deletion after the shader program is deleted
  GLCall(glDeleteShader(vso));
  GLCall(glDeleteShader(fso));
}


Shader::Shader()
: _vid(0), _fid(0){
  GLCall(_id = glCreateProgram());
}

Shader::~Shader(){
  GLCall(glDeleteProgram(_id));
}

void Shader::Frag(const char* string, bool isPath, bool recompile){
  std::string src;
  if (isPath){
    src = ReadRel(string);
  } else{
    src = string;
  }

  _fid = CreateShader(GL_FRAGMENT_SHADER, src);

  if (this->IsValid() && recompile){
    LinkProgram(_id, _vid, _fid);
  }

  _locationCache.clear();
}

void Shader::Vert(const char* string, bool isPath, bool recompile){
  std::string src;
  if (isPath){
    src = ReadRel(string);
  } else{
    src = string;
  }

  _vid = CreateShader(GL_VERTEX_SHADER, src);

  if (this->IsValid() && recompile){
    LinkProgram(_id, _vid, _fid);
  }

  _locationCache.clear();
}

void Shader::Bind() const{
  GLCall(glUseProgram(_id));
}

void Shader::Unbind() const{
  GLCall(glUseProgram(0));
}

bool Shader::IsValid() const{
  return _vid != 0 && _fid != 0;
}

//Uniforms
int Shader::_GetUniformLocation(const std::string& name){
  this->Bind();

  if (_locationCache.contains(name)){
    return _locationCache.at(name);
  }

  GLCall(int location = glGetUniformLocation(_id, name.c_str()));

  if (location == -1){
    HT_LOG_WARNING("Uniform (", name, ") couldnt be found");
  }
  else{
    _locationCache.emplace(name, location);
  }

  return location;
}

void Shader::SetUniform(const std::string& name, const glm::mat4& mat){
  int location = _GetUniformLocation(name);

  GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
  //Location
  //Count
  //Transpouse, whether to convert to row-major to column-major, glm stores them column-major
  //Address
}

void Shader::SetUniform(const std::string& name, float f0, float f1, float f2, float f3){
  int location = _GetUniformLocation(name);
  GLCall(glUniform4f(location, f0, f1, f2, f3));
}

void Shader::SetUniform(const std::string& name, float f0){
  int location = _GetUniformLocation(name);
  GLCall(glUniform1f(location, f0));
}

void Shader::SetUniform(const std::string& name, int i0){
  int location = _GetUniformLocation(name);
  GLCall(glUniform1i(location, i0));
}

void Shader::SetUniform(const std::string& name, int is[], int count){
  int location = _GetUniformLocation(name);
  GLCall(glUniform1iv(location, count, is));
}

}
