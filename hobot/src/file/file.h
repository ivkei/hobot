#pragma once

#include<string>
#include<sstream>
#include<fstream>
#include<filesystem>
#include<limits.h>

#include"api.h"

namespace hobot{

//Gets dir of exec file
std::filesystem::path HOBOT_API GetExecDir();

//Takes in path relative to the exec's dir
std::string HOBOT_API ReadRel(std::string relPath);

}
