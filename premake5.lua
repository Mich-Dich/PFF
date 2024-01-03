-- premake5.lua

project_name = "Sandbox"										-- This is the name of your project
outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"		-- output direactory of complie results

workspace "PFF"
	platforms "x64"
    startproject (project_name)

	configurations
	{
		"Debug",
		"Release"
	}

	IncludeDir = {}				-- Include directories relative to root folder (solution directory)
	IncludeDir["ImGui"] = "PFF/vendor/ImGui"
	IncludeDir["glm"] = "PFF/vendor/glm"

	include "PFF/vendor/ImGui"

project "PFF"
	location "PFF"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	-- pchheader "pffpch.h"
	-- pchsource "PFF/src/pffpch.cpp"

	defines
	{
		"ENGINE_NAME=PFF",
		'PROJECT_NAME="' .. project_name .. '"',
		"PFF_INSIDE_ENGINE",
		"GLFW_INCLUDE_NONE",
	}

	files
    {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"PFF/vendor/glm",
		"PFF/vendor/glfw-3.3.8.bin.WIN64/include",
		
		"C:/VulkanSDK/1.3.250.1/Include", -- CHANGE THIS
	}
	
	libdirs 
	{
		"PFF/vendor/glfw-3.3.8.bin.WIN64/lib-vc2022",

		"C:/VulkanSDK/1.3.250.1/Lib", -- CHANGE THIS
	}

	links 
	{
		"vulkan-1.lib",
		"glfw3.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		defines { "_CRT_STDIO_ISO_WIDE_SPECIFIERS" } -- Enable C17 features for Visual Studio
		staticruntime "On"
		systemversion "latest"
		
        linkoptions 
		{
			 "/NODEFAULTLIB:LIBCMTD",
			 "/NODEFAULTLIB:MSVCRT",
		}

		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		postbuildcommands
		{
			"{MKDIR} ../bin/" .. outputs .. "/" .. project_name,
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputs  .. "/" .. project_name,
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "PFF_DEBUG"
		symbols "On"

	filter "configurations:Release"
		buildoptions "/MD"
		defines "PFF_RELEASE"
		optimize "On"

project (project_name)
	location (project_name)
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	defines
	{
		"ENGINE_NAME=PFF",
		"PROJECT_NAME=%{project_name}",
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs
	{
		"PFF/src",
	}

	links
	{
		"PFF",
	}

	filter "system:windows"
		cppdialect "C++17"
		defines { "_CRT_STDIO_ISO_WIDE_SPECIFIERS" } -- Enable C17 features for Visual Studio
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "GAME_DEBUG"
		symbols "On"

	filter "configurations:Release"
		buildoptions "/MD"
		defines "GAME_RELEASE"
		optimize "On"
