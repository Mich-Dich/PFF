workspace "PFF"
	platforms "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Include directories relative to root folder (solution directory)
	IncludeDir = {}
	IncludeDir["SDL2"] = "%{prj.name}/vendor/SDL2/include"
	IncludeDir["glad"] = "%{prj.name}/vendor/glad/include"

	Lib_Dir = "lib"
	-- include "PFF/vendor/glad"

project "PFF"
	location "PFF"
	kind "SharedLib"
	language "C"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	-- pchheader "pffpch.h"
	-- pchsource "PFF/src/pffpch.cpp"

	files
    {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c"
	}

      -- Specify the directory where the DLL is located
    libdirs
	{
		"DLL"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{IncludeDir.SDL2}",
		"%{IncludeDir.glad}",
		-- "%{IncludeDir.GLFW}",
		-- "%{IncludeDir.ImGui}"
	}
	
	links 
	{
		"%{Lib_Dir}/SDL2.lib",
		"%{Lib_Dir}/SDL2main.lib",
		"%{Lib_Dir}/SDL2_mixer.lib",
		"%{Lib_Dir}/SDL2test.lib",
		"%{Lib_Dir}/freetype.lib"
	}

	filter "system:windows"
		defines { "_CRT_STDIO_ISO_WIDE_SPECIFIERS" } -- Enable C17 features for Visual Studio
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS",
			"PFF_INSIDE_ENGINE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputs  .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "PFF_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.c"
	}

	includedirs
	{
		"PFF/src",
		"vendor"
	}

	links
	{
		"PFF"
	}

	filter "system:windows"
		defines { "_CRT_STDIO_ISO_WIDE_SPECIFIERS" } -- Enable C17 features for Visual Studio
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "PFF_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		buildoptions "/MD"
		optimize "On"
