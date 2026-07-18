#pragma once

#include"hobot.h"

class App : public hobot::Application{
private:
  std::unique_ptr<hobot::Window> _pWindow;
public:
  App();
  virtual ~App() override;
  virtual void Run() override;
};
