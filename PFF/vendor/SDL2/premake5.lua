project "SDL"
	kind "StaticLib"
	staticruntime "off"
	location         ""

	configurations
	{
		"Debug",
		"Release"
	}

	platforms
	{
		"x64"
	}

	filter "platforms:x64"
		architecture "x64"

	filter
	{}

	include "SDL2.lua"

	filter "system:windows"                 -- Only needed on Windows for WinMain().
		include "SDL2main.lua"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"
