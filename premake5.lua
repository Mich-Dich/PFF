-- premake5.lua

client_project_name = "Sandbox"						-- This is the name of your project
vulkan_dir = "C:/VulkanSDK/1.3.250.1"			-- CHANGE THIS

workspace "PFF"
	platforms "x64"
    startproject "PFF_editor"

	configurations
	{
		"Debug",
		"Release"
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["glm"] = "PFF/vendor/glm"
	IncludeDir["ImGui"] = "PFF/vendor/imgui"

	include "PFF/vendor/imgui"

project "PFF"
	location "PFF"
	kind "SharedLib"	
	staticruntime "off"
	language "C++"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")
	
	pchheader "util/pffpch.h"
	pchsource "PFF/src/util/pffpch.cpp"

	defines
	{
		"ENGINE_NAME=PFF",
		'PROJECT_NAME="' .. client_project_name .. '"',
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
		"%{prj.name}/assets",
		"PFF/vendor/glm",
		"PFF/vendor/glfw-3.3.8.bin.WIN64/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		
		vulkan_dir .. "/Include",
	}

	links 
	{
		"ImGui",
		"vulkan-1.lib",
		"glfw3.lib",
	}
	
	libdirs 
	{
		--"PFF/libs",
		--"%{IncludeDir.ImGui}/bin/" .. outputs .. "/ImGui",
		"PFF/vendor/glfw-3.3.8.bin.WIN64/lib-vc2022",
		vulkan_dir .. "/Lib",
	}
	
	filter "system:windows"
		cppdialect "C++17"
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
			"{MKDIR} ../bin/" .. outputs .. "/" .. client_project_name,
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputs  .. "/" .. client_project_name,
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "PFF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		buildoptions "/MD"
		defines "PFF_RELEASE"
		runtime "Release"
		optimize "on"

project (client_project_name)
	location (client_project_name)
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")
	
	glslc = "../PFF/vendor/vulkan-glslc/glslc.exe"

	defines
	{
		"ENGINE_NAME=PFF",
		"PROJECT_NAME=%{client_project_name}",
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/**.vert",
		"%{prj.name}/**.frag",
	}
	
	includedirs
	{
		"PFF/vendor/glm",
		"PFF/src",
	}

	links
	{
		"PFF",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		postbuildcommands
		{
			'"%{glslc}" shaders/default.vert -o shaders/default.vert.spv',
			'"%{glslc}" shaders/default.frag -o shaders/default.frag.spv',
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "GAME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		buildoptions "/MD"
		defines "GAME_RELEASE"
		runtime "Release"
		optimize "on"

project "PFF_editor"
	location "PFF_editor"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")
	
	glslc = "../PFF/vendor/vulkan-glslc/glslc.exe"

	pchheader "util/pffpch.h"
	pchsource "PFF/src/util/pffpch.cpp"

	defines
	{
		"ENGINE_NAME=PFF",
		'PROJECT_NAME=PFF_editor',
		"PFF_INSIDE_EDITOR",
		"GLFW_INCLUDE_NONE",
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{client_project_name}/src/**.h",
		"%{client_project_name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"PFF/vendor/glm",
		"PFF/vendor/glfw-3.3.8.bin.WIN64/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		
		vulkan_dir .. "/Include",
	}

	links 
	{
		"PFF",
		"ImGui",
		"vulkan-1.lib",
		"glfw3.lib",
		client_project_name,
	}
	
	libdirs 
	{
		--"PFF/libs",
		--"%{IncludeDir.ImGui}/bin/" .. outputs .. "/ImGui",
		"PFF/vendor/glfw-3.3.8.bin.WIN64/lib-vc2022",
		vulkan_dir .. "/Lib",
	}
	
	filter "system:windows"
		cppdialect "C++17"
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
			"{MKDIR} ../bin/" .. outputs .. "/" .. client_project_name,
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputs  .. "/" .. client_project_name,
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "PFF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		buildoptions "/MD"
		defines "PFF_RELEASE"
		runtime "Release"
		optimize "on"
