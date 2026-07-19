#include"hobot.h"

#include"notesapp/notesapp.h"

extern std::unique_ptr<hobot::Application> hobot::CreateApplication(){
  return std::make_unique<NotesApp>();
}

HOBOT_ENTRY_POINT

//TODO: continue with shaders chapter
//TODO: more abstractions
