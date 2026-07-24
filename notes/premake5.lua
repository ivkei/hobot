workspace "notes"
architecture "x64"
configurations
{
  "Debug"
}
targetdir("../build/bin/")
objdir("../build/bin-int/")
cppdialect "C++23"

include "../hobot/"

project "notes"
  kind "ConsoleApp"
  language "C++"
  staticruntime "On"

  absCompileTimeRes = path.getabsolute("res/")
  relRunTimeRes = path.getrelative("../build/bin/", "../build/res/") -- Relative to binary

  files
  {
    "src/**.cpp",
  }

  defines
  {
    "RES_DIR=\"" .. relRunTimeRes .. "/\"",
  }

  postbuildcommands{
    "{COPYDIR} \"" .. absCompileTimeRes .. "\" \"" .. "../build/" .. "\""
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
