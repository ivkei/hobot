#pragma once

#include"hobot.h"

class App : public hobot::Application{
private:
  std::unique_ptr<hobot::Window> _pWindow;
  bool _pause;
  float _pauseTimer;
  float _pauseCD;
public:
  App();
  virtual ~App() override;
  virtual void Run() override;
};
