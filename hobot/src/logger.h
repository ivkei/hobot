#pragma once

#include<iostream>
#include<string>
#include<mutex>
#include<chrono>
#include<iomanip>

namespace hobot{

#ifndef _HOBOT_WINDOWS
static std::mutex localtimeFuncLock;
#endif

//This function gets current timestamp
static auto GetTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  
#ifdef _HOBOT_WINDOWS
  tm time;
  localtime_s(&time, &in_time_t);
  ss << std::put_time(&time, "[%Y-%m-%d, %H:%M:%S]");
#else
  localtimeFuncLock.lock();
  ss <<  std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d, %H:%M:%S]");
  localtimeFuncLock.unlock();
#endif
  return ss.str();
}


template<class...Ts>
inline void Log(const std::string& preInfo, const std::string& color, Ts...args){
  std::cout << color << GetTimestamp() << " (" << preInfo << ") ";
  ((std::cout << std::forward<Ts>(args)), ...);
  std::cout << "\033[0m" << std::endl;
}

#ifdef _HOBOT_DEBUG
#define _HOBOT_ENABLE_LOGGING
#define _HOBOT_ENABLE_ASSERTS
#endif

#ifdef _HOBOT_ENABLE_LOGGING
  #ifdef _HOBOT_ENGINE
    #define HT_LOG_INFO(...)         hobot::Log("Hobot Info", "\033[0m", __VA_ARGS__)
    #define HT_LOG_WARNING(...)      hobot::Log("Hobot Warning", "\033[33m", __VA_ARGS__)
    #define HT_LOG_ERROR(...)        hobot::Log("Hobot Error", "\033[31m", __VA_ARGS__)
    #define HT_LOG_SUCCESS(...)      hobot::Log("Hobot Success", "\033[32m", __VA_ARGS__)
    #ifdef _HOBOT_ENABLE_ASSERTS
      #define HT_LOG_ASSERT(x, ...) if (!(x)) hobot::Log("Hobot Assertion failed (" #x ")", "\033[34m", __VA_ARGS__, "\n{\nFile: ", __FILE__, "\nLine: ", __LINE__, "\n}")
    #else
      #define HT_LOG_ASSERT(...)
    #endif
  #else
    #define HT_LOG_INFO(...)         hobot::Log("Info", "\033[0m", __VA_ARGS__)
    #define HT_LOG_WARNING(...)      hobot::Log("Warning", "\033[33m", __VA_ARGS__)
    #define HT_LOG_ERROR(...)        hobot::Log("Error", "\033[31m", __VA_ARGS__)
    #define HT_LOG_SUCCESS(...)      hobot::Log("Success", "\033[32m", __VA_ARGS__)
    #ifdef _HOBOT_ENABLE_ASSERTS
      #define HT_LOG_ASSERT(x, ...) if (!(x)) hobot::Log("Assertion failed (" #x ")", "\033[34m", __VA_ARGS__, "\n{\nFile: ", __FILE__, "\nLine: ", __LINE__, "\n}")
    #else
      #define HT_LOG_ASSERT(...)
    #endif
  #endif
#else
  #define HT_LOG_INFO(...)
  #define HT_LOG_WARNING(...)
  #define HT_LOG_ERROR(...)
  #define HT_LOG_SUCCESS(...)
  #define HT_LOG_ASSERT(x, ...)
#endif

}
