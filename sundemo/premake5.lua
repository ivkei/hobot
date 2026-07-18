workspace "sundemo"
architecture "x64"
configurations
{
  "Debug"
}
targetdir("../bin/")
objdir("../bin-int/")
cppdialect "C++23"

include "../hobot/"

project "sundemo"
  kind "ConsoleApp"
  language "C++"
  staticruntime "On"

  files
  {
    "src/**.cpp",
  }

  links
  {
    "hobot",
  }

  includedirs
  {
    "src/",
    "../hobot/include",
    "../hobot/vendors/glm-1.0.1",
    "../hobot/src/",
  }

  filter "system:windows"
    defines
    {
      "_HOBOT_WINDOWS"
    }
  filter "system:linux"
    defines
    {
      "_HOBOT_LINUX"
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"
    defines "_HOBOT_DEBUG"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"
    defines "_HOBOT_RELEASE"
