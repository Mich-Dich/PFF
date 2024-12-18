
project "PFF_helper"
	location "%{wks.location}/PFF_helper"
	kind "ConsoleApp"
	staticruntime "off"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	defines
	{
		"PFF_HELPER_PROGRAM",
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
		"PFF",
	}

	libdirs 
	{
		"%{wks.location}/bin/" .. outputs  .. "/PFF"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		files					-- Include the icon resource only for Windows
		{
			"../metadata/app_icon.rc",
		}

	filter "configurations:Debug"
		defines "PFF_EDITOR_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:RelWithDebInfo"
		defines "PFF_EDITOR_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "On"

	filter "configurations:Release"
		defines "PFF_EDITOR_RELEASE"
		runtime "Release"
		symbols "off"
		optimize "on"
