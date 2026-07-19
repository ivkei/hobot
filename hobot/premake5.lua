include "vendors/glew-2.2.0"
include "vendors/glfw-3.4"

project  "hobot"
  language "C++"
  cppdialect "C++23"
  staticruntime "On" -- Link standard libraries statically
  kind "SharedLib"
  pic "on" -- Position independent code
  -- DLL feature that changes the way functions are addressed
  -- Because DLL instructions dont change but get loaded into a ton of different VASs

  pchheader "src/ht_pch/pch.h"
  pchsource "src/ht_pch/pch.cpp"

  files{
    "src/**.cpp",
  }

  defines{
    "_CRT_SECURE_NO_WARNINGS",
    "GLEW_STATIC",
    "_HOBOT_ENGINE"
  }

  links{
    "glfw",
    "glew"
  }

  includedirs{
    "vendors/glfw-3.4/include",
    "vendors/glew-2.2.0/include",
    "vendors/glm-1.0.1",
    "src",
  }

  filter "system:windows"
    defines{
      "_GLFW_WIN32",
      "_HOBOT_WINDOWS"
    }
    links{
      "OpenGL32",
      "Gdi32"
    }

  filter "system:linux"
    defines
    {
      "_GLFW_X11",
      "_HOBOT_LINUX",
    }
    links
    {
      "GL",
      "GLU",
      "pthread",
      "Xrandr",
      "Xcursor",
      "Xi",
      "X11",
      "dl",
    }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"
    defines "_HOBOT_DEBUG"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"
    defines "_HOBOT_RELEASE"

