
project "PFF"
	location "%{wks.location}/PFF"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")
	
	pchheader "util/pffpch.h"
	pchsource "src/util/pffpch.cpp"

	defines
	{
		"PFF_ENGINE",
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.embed",
		"**..pffproj",
		"**..pff",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs
	{
		"src",
		"assets",
        
		"%{IncludeDir.glm}",
		"%{IncludeDir.glfw}/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",		
		"%{IncludeDir.VulkanSDK}",
	}
	
	links
	{
		"glfw",
		"ImGui",
        "%{Library.Vulkan}",
	}

	libdirs 
	{
		"vendor/imgui/bin/Debug-windows-x86_64/ImGui",
        "%{IncludeDir.VulkanSDK}/lib",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	
	filter "system:windows"
		systemversion "latest"
		
		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		postbuildcommands
		{
			-- copy premake exe (needed for engine projects)
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"{COPY} %{wks.location}/vendor/premake %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			
			-- postbuildcommands { table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/defaults", "PFF/assets"})), }
			table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/assets"})),
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

	-- filter { "system:linux", "action:gmake" }
  	-- 	buildoptions { "`wx-config --cxxflags`", "-ansi" }
		  
	filter "system:linux"
		systemversion "latest"
		defines "PFF_PLATFORM_LINUX"
		-- toolset "clang"
		
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

		buildoptions { "-msse4.1" }  -- Add this line to include the SSE4.1 flag for Linux builds

		postbuildcommands
		{
			-- Create the directory if it doesn't exist
			"mkdir -p %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"cp %{wks.location}/premake5 %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			
			-- Copy content of directories
			table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/assets"})),
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
