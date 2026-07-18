#include"hobot.h"

#include"notesapp/notesapp.h"

extern std::unique_ptr<hobot::Application> hobot::CreateApplication(){
  return std::make_unique<NotesApp>();
}

HOBOT_ENTRY_POINT

//TODO: After finished section make into a batch renderer (abstract away as much as possible)
//TODO: continue with shaders chapter
