#include"hobot.h"

#include"app/app.h"

extern std::unique_ptr<hobot::Application> hobot::CreateApplication(){
  return std::make_unique<App>();
}

HOBOT_ENTRY_POINT
