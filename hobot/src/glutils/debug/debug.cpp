#include"pch/pch.h"

#include"debug.h"

#include"GL/glew.h"

namespace hobot{

static const char* GLSourceToString(GLenum source){
  switch (source) {
    case GL_DEBUG_SOURCE_API:
    return "API";

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "WINDOW SYSTEM";

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "SHADER COMPILER";

    case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "THIRD PARTY";

    case GL_DEBUG_SOURCE_APPLICATION:
    return "APPLICATION";

    case GL_DEBUG_SOURCE_OTHER:
    return "UNKNOWN";

    default:
    return "UNKNOWN";
  }
}

static const char* GLTypeToString(GLenum type){
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
    return "ERROR";

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "DEPRECATED BEHAVIOR";

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "UDEFINED BEHAVIOR";

    case GL_DEBUG_TYPE_PORTABILITY:
    return "PORTABILITY";

    case GL_DEBUG_TYPE_PERFORMANCE:
    return "PERFORMANCE";

    case GL_DEBUG_TYPE_OTHER:
    return "OTHER";

    case GL_DEBUG_TYPE_MARKER:
    return "MARKER";

    default:
    return "UNKNOWN";
  }
}

static const char* GLSeverityToString(GLenum severity){
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
    return "HIGH";

    case GL_DEBUG_SEVERITY_MEDIUM:
    return "MEDIUM";

    case GL_DEBUG_SEVERITY_LOW:
    return "LOW";

    case GL_DEBUG_SEVERITY_NOTIFICATION:
    return "NOTIFICATION";

    default:
    return "UNKNOWN";
  }
}

static void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data){
  const char* sourceStr = GLSourceToString(source);
  const char* typeStr = GLTypeToString(type);
  const char* severityStr = GLSeverityToString(severity);

  if (std::strcmp(typeStr, "OTHER")) { //Filter some info
    HT_LOG_ERROR(id, ": ", typeStr, " of ", severity, ", raised from ", sourceStr, ": ", msg);
  }
}

void GLEnableAutoLogging(){
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(GLDebugMessageCallback, NULL);
  //NULL here is for custom pointer to be passed to every callback
}

void GLClearErrors(){
  while (glGetError());
}

static const char* GLErrorToString(GLenum error){
  switch (error){
    case GL_INVALID_ENUM:                  return "INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "INVALID_OPERATION";
    case GL_STACK_OVERFLOW:                return "STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:               return "STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:                 return "OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
    default:                               return "UNKNOWN";
  }
}

void GLLogErrors(const char* command, const char* file, int line){
  while (GLenum error = glGetError()){
    HT_LOG_ERROR(GLErrorToString(error));
  }
}

}
