-- premake5.lua
workspace "seam-carving"
   configurations { "Debug", "Release" }
   location "build"

project "seam-carving"
   kind "ConsoleApp"

   language "C++"

   files { "**.hpp", "**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
