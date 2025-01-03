
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


	filter "system:linux"
		systemversion "latest"
		defines { "PFF_PLATFORM_LINUX" }
	
		includedirs {
			"/usr/include/gtk-3.0",
			"/usr/include/atk-1.0",			-- needed for GTK
			"/usr/include/gdk-pixbuf-2.0",	-- needed for GTK
			"/usr/include/harfbuzz",		-- needed for GTK
			"/usr/include/cairo",			-- needed for GTK
			"/usr/include/glib-2.0",
			"/usr/include/pango-1.0",
			"/usr/lib/x86_64-linux-gnu/glib-2.0/include", -- GLib include path
		}
	
		libdirs {
			"/usr/lib/x86_64-linux-gnu", -- Library directory for linking
		}
	
		links {
			"gtk-3",  						-- Link against GTK
			"gdk-3",  						-- Link against GDK
			"glib-2.0", 					-- Link against GLib
			"gobject-2.0", 					-- Link against GObject
			"gio-2.0", 						-- Link against GIO
			"gmodule-2.0", 					-- Link against GModule
			"gthread-2.0", 					-- Link against GThread
			"pango-1.0", 					-- Link against Pango
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