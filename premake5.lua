-- premake5.lua

workspace "PFF"
	platforms "x64"
    startproject "Sandbox" -- Replace with the actual project name

	configurations
	{
		"Debug",
		"Release"
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Include directories relative to root folder (solution directory)
	IncludeDir = {}
	IncludeDir["GLFW"] = "PFF/vendor/GLFW/include"
	IncludeDir["glad"] = "PFF/vendor/glad/include"
	IncludeDir["ImGui"] = "PFF/vendor/ImGui"

	include "PFF/vendor/GLFW"
	include "PFF/vendor/glad"
	include "PFF/vendor/ImGui"

project "PFF"
	location "PFF"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	-- pchheader "pffpch.h"
	-- pchsource "PFF/src/pffpch.cpp"

	files
    {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		
		"C:/VulkanSDK/1.3.250.1/Include", -- CHANGE THIS
		"C:/Users/mally/Documents/Visual Studio 2022/Libraries/glm",
		"C:/Users/mally/Documents/Visual Studio 2022/Libraries/glfw-3.3.8.bin.WIN64/include"
		
		--"%{IncludeDir.GLFW}",
		--"%{IncludeDir.glad}",
		-- "%{IncludeDir.ImGui}"
	}
	
	libdirs 
	{
		"C:/VulkanSDK/1.3.250.1/Lib",
		"C:/Users/mally/Documents/Visual Studio 2022/Libraries/glfw-3.3.8.bin.WIN64/lib-vc2022",
	}

	links 
	{
		"vulkan-1.lib",
		"glfw3.lib",
		--"GLFW",
		--"glad",
		--"opengl32.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		defines { "_CRT_STDIO_ISO_WIDE_SPECIFIERS" } -- Enable C17 features for Visual Studio
		staticruntime "On"
		systemversion "latest"

        -- Add /NODEFAULTLIB:LIBCMTD to the linker options
        linkoptions { "/NODEFAULTLIB:LIBCMTD" }

		defines
		{
			"PFF_PLATFORM_WINDOWS",
			"PFF_INSIDE_ENGINE",
			"GLFW_INCLUDE_NONE"
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
	language "C++"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	includedirs
	{
		"PFF/src",
		--"%{IncludeDir.GLFW}",
		--"%{IncludeDir.glad}",
		-- "%{IncludeDir.ImGui}"
	}

	links
	{
		"PFF",
		--"GLFW",
		--"glad",
		--"opengl32.lib",
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
		defines "PFF_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		buildoptions "/MD"
		optimize "On"
