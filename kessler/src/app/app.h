#pragma once

#include"hobot.h"

#include"game/game.h"

class App : public hobot::Application{
private:
  std::unique_ptr<hobot::Window> _pWindow;
  Game _game;
public:
  App();
  virtual ~App() override;
  virtual void Run() override;
};
