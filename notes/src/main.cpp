#include"hobot.h"

#include"notesapp/notesapp.h"

extern std::unique_ptr<hobot::Application> hobot::CreateApplication(){
  return std::make_unique<NotesApp>();
}

HOBOT_ENTRY_POINT

//TODO: add texture support to hobot (sprites?)
//TODO: Stopped at mipmaps
//TODO: more abstractions
