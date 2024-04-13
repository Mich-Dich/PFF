
project "PFF"
	location "%{wks.location}/PFF"
	kind "SharedLib"
	staticruntime "off"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")
	
	pchheader "util/pffpch.h"
	pchsource "src/util/pffpch.cpp"

	defines
	{
		"ENGINE_NAME=PFF",
		'PROJECT_NAME="' .. client_project_name .. '"',
		"PFF_INSIDE_ENGINE",
		"GLFW_INCLUDE_NONE",
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"assets",
        
		"%{IncludeDir.glm}",
		"%{IncludeDir.glfw}/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		"%{IncludeDir.tinyobjloader}",
		"%{IncludeDir.stb_image}",

		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.vma}",
		"%{IncludeDir.vkbootstrap}",
	}
	
	links 
	{
		"glfw",
		"ImGui",
        "%{Library.Vulkan}"
	}

	libdirs 
	{
		--"PFF/libs",
		--"%{IncludeDir.ImGui}/bin/" .. outputs .. "/ImGui",
		--   "PFF/vendor/glfw/lib-vc2022",

        "%{IncludeDir.VulkanSDK}/lib"
	}
	
	filter "system:windows"
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
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/" .. client_project_name,
			"{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputs  .. "/" .. client_project_name,
			
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/PFF_editor",
			"{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputs  .. "/PFF_editor",
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "PFF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:RelWithDebInfo"
		defines "PFF_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "speed"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		runtime "Release"
		optimize "on"
