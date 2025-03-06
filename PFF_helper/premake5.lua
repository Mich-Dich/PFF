
project "PFF_helper"
	location "%{wks.location}/PFF_helper"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	defines { "PFF_HELPER", }

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
		defines {
			"PFF_PLATFORM_WINDOWS",
			"GLSLC_PATH=\"%{VULKAN}/Bin/glslc.exe\"",
		}
		files { "../metadata/app_icon.rc" }

	filter "system:linux"
		systemversion "latest"
		defines {
			"PFF_PLATFORM_LINUX",
			"GLSLC_PATH=\"/usr/bin/glslc\"",
		}
	
		includedirs {
			"/usr/include/x86_64-linux-gnu/qt5", 				-- Base Qt include path
			"/usr/include/x86_64-linux-gnu/qt5/QtCore", 		-- Specific QtCore include path
			"/usr/include/x86_64-linux-gnu/qt5/QtWidgets", 		-- Specific QtWidgets include path
			"/usr/include/x86_64-linux-gnu/qt5/QtGui", 			-- Specific QtGui include path
		}
	
		libdirs {
			"/usr/lib/x86_64-linux-gnu", -- Library directory for linking
			"/usr/lib/x86_64-linux-gnu/qt5",
		}
	
		links {
			"Qt5Core",    -- Link against Qt5Core
			"Qt5Widgets", -- Link against Qt5Widgets
			"Qt5Gui",     -- Link against Qt5Gui
		}
		
	filter "configurations:Debug"
		defines "PFF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:RelWithDebInfo"
		defines "PFF_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "on"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		runtime "Release"
		symbols "off"
		optimize "on"