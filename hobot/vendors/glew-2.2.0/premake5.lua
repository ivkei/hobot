project "glew"
kind "StaticLib"
language "C"
pic"on"

--targetdir("../../bin/")
--objdir("../../bin-int/")

files
{
  "src/glew.c",
  "include/**.h"
}

includedirs
{
  "include"
}

defines
{
  "GLEW_STATIC",
  "_CRT_SECURE_NO_WARNINGS"
}
