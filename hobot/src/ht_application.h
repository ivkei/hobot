#pragma once

#include<memory>

namespace hobot{
  struct Application{
    Application() {}
    virtual ~Application() {}
    virtual void Run() {}
  };

  extern std::unique_ptr<Application> CreateApplication();

}

#define HOBOT_ENTRY_POINT\
  int main(){\
    HT_LOG_INFO("Exec main");\
    std::unique_ptr<hobot::Application> pApp = hobot::CreateApplication();\
    pApp->Run();\
    return 0;\
  }
