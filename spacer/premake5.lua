workspace "spacer"
architecture "x64"
configurations
{
  "Debug",
  "Release",
  "Dist"
}
targetdir("../build/%{cfg.buildcfg}/bin/")
objdir("../build/bin-int/") -- Already have cfg distinction
cppdialect "C++23"

include "../hobot/"

project "spacer"
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
    kind "ConsoleApp"

  filter "configurations:Release"
    runtime "Release"
    optimize "full"
    linktimeoptimization"On"
    defines "_HOBOT_DEBUG"
    kind "ConsoleApp"

  filter "configurations:Dist"
    runtime "Release"
    optimize "full"
    linktimeoptimization"On"
    defines "_HOBOT_RELEASE"
    kind "WindowedApp"

--NOTE: my most complete premake5.lua
