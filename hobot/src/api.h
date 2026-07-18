#pragma once

#ifdef _HOBOT_WINDOWS
  #ifdef _HOBOT_ENGINE
    #define HOBOT_API __declspec(dllexport)
  #else
    #define HOBOT_API __declspec(dllimport)
  #endif
#elif _HOBOT_LINUX
  #define HOBOT_API
#else
  #error "Unknown Platform"
#endif
