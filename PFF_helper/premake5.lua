
project "PFF_helper"
	location "%{wks.location}/PFF_helper"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	defines
	{
		"PFF_HELPER",
		"GLSLC_EXE=\"%{VULKAN_SDK}/Bin/glslc.exe\"",
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{wks.location}/PFF/src",

		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
	}

	links
	{
		"PFF"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines { "PFF_PLATFORM_WINDOWS" }
		files { "../metadata/app_icon.rc" }

	filter "configurations:Debug"
		defines "PFF_HELPER_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:RelWithDebInfo"
		defines "PFF_HELPER_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "On"

	filter "configurations:Release"
		defines "PFF_HELPER_RELEASE"
		runtime "Release"
		symbols "off"
		optimize "on"
