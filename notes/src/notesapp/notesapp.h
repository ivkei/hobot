#pragma once

#include"hobot.h"

class NotesApp : public hobot::Application{
private:
  std::unique_ptr<hobot::Window> _pWindow;
public:
  NotesApp();
  virtual ~NotesApp() override;
  virtual void Run() override;
};
