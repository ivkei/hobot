project "stb_image"
kind "StaticLib"
staticruntime "On" -- Link standard libraries statically
language "C"
pic"on"

files
{
  "stb_image.cpp",
}
