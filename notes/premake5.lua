workspace "notes"
architecture "x64"
configurations
{
  "Debug",
  "Release"
}
targetdir("../build/%{cfg.buildcfg}/bin/")
objdir("../build/bin-int/")
cppdialect "C++23"

include "../hobot/"

project "notes"
  kind "ConsoleApp"
  language "C++"
  staticruntime "On"

  absCompileTimeRes = path.getabsolute("res/")
  relRunTimeRes = path.getrelative("../build/%{cfg.buildcfg}/bin/", "../build/%{cfg.buildcfg}/res/") -- Relative to binary

  files
  {
    "src/**.cpp",
  }

  defines
  {
    "RES_DIR=\"" .. relRunTimeRes .. "/\"",
  }

  postbuildcommands{
    "{COPYDIR} \"" .. absCompileTimeRes .. "\" \"" .. "../build/%{cfg.buildcfg}/" .. "\""
  }

  links
  {
    "hobot",
    "glfw",
    "glew"
  }

  includedirs
  {
    "src/",
    "../hobot/include",
    "../hobot/vendors/glm-1.0.1",
    "../hobot/vendors/glew-2.2.0/include",
    "../hobot/vendors/glfw-3.4/include",
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
    optimize "full"
    linktimeoptimization "on"
    defines "_HOBOT_RELEASE"
