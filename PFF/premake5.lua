
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
		"%{IncludeDir.Vulkan}",
	}
	
	links
	{
		"ImGui",
		"glfw",
	}



	libdirs 
	{
		"vendor/imgui/bin/" .. outputs .. "/ImGui",
		"%{Library.Vulkan}",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }
	
	filter "system:windows"
		systemversion "latest"		
		defines "PFF_PLATFORM_WINDOWS"

		links
		{
			"glfw",
			"%{Library.Vulkan}",
		}

		libdirs 
		{
			"vendor/imgui/bin/" .. outputs .. "/ImGui",
			-- "%{IncludeDir.Vulkan}/Lib",
		}
	
		postbuildcommands {
			
			-- copy resources
			'{COPYDIR} "%{wks.location}/PFF/shaders" "%{wks.location}/bin/' .. outputs .. '/PFF/shaders"',
			'{COPYDIR} "%{wks.location}/PFF/assets" "%{wks.location}/bin/' .. outputs .. '/PFF/assets"',
			'{COPYDIR} "%{wks.location}/PFF/src" "%{wks.location}/bin/' .. outputs .. '/PFF/src"',
			'{COPY} "%{wks.location}/dependencies.lua" "%{wks.location}/bin/' .. outputs .. '/"',
			
			-- Ensure the destination directory exists before copying
			
			-- copy vendor dependencies
			'{MKDIR} "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
			'{COPY} "%{wks.location}/PFF/vendor/entt/LICENSE.txt" "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
			'{COPYDIR} "%{wks.location}/PFF/vendor/entt/include" "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
			
			'{MKDIR} "%{wks.location}/bin/' .. outputs .. '/vendor/glm"',
			'{COPYDIR} "%{wks.location}/PFF/vendor/glm/glm" "%{wks.location}/bin/' .. outputs .. '/vendor/glm/glm"',
			
			-- copy GLFW
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/glfw",
			'{COPYDIR} "%{wks.location}/PFF/vendor/glfw/bin/' .. outputs .. '/GLFW" "%{wks.location}/bin/' .. outputs .. '/vendor/glfw"',
			'{COPY} "%{wks.location}/PFF/vendor/glfw/LICENSE.md" "%{wks.location}/bin/' .. outputs .. '/vendor/glfw"',
						
			-- copy premake exe (needed for engine projects)
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"{COPY} %{wks.location}/vendor/premake %{wks.location}/bin/" .. outputs .. "/vendor/premake",
		}
		  
	filter "system:linux"
		systemversion "latest"
		defines "PFF_PLATFORM_LINUX"

		-- -- ANSI escape codes for colors
		-- local RESET = "\27[0m"
		-- local GREEN = "\27[32m"

		-- prebuildmessage ("================= Building PFF %{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture} =================")
		-- postbuildmessage ("================= Done building PFF %{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture} =================")
		
		includedirs
		{
			"/usr/include/x86_64-linux-gnu/qt5", 				-- Base Qt include path
			"/usr/include/x86_64-linux-gnu/qt5/QtCore",
			"/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
			"/usr/include/x86_64-linux-gnu/qt5/QtGui",
			"%{IncludeDir.VulkanUtils}",
		}
	
		libdirs
		{
			"%{wks.location}/PFF/vendor/glfw/build/src",
			"/usr/lib/x86_64-linux-gnu",
			"/usr/lib/x86_64-linux-gnu/qt5",
		}
	
		links
		{
			"vulkan",
			"glfw",
			"Qt5Core",
			"Qt5Widgets",
			"Qt5Gui",
		}

		buildoptions {
			"-msse4.1",										  	-- include the SSE4.1 flag for Linux builds
			"-fPIC",
		}

		postbuildcommands {

			-- copy resources
			'{COPYDIR} "%{wks.location}/PFF/shaders" "%{wks.location}/bin/' .. outputs .. '/PFF"',
			'{COPYDIR} "%{wks.location}/PFF/assets" "%{wks.location}/bin/' .. outputs .. '/PFF"',
			'{COPYDIR} "%{wks.location}/PFF/src" "%{wks.location}/bin/' .. outputs .. '/PFF/src"',
			'{COPY} "%{wks.location}/dependencies.lua" "%{wks.location}/bin/' .. outputs .. '/"',
	
			-- Ensure the destination directory exists before copying
			
			-- copy vendor dependencies
			'{MKDIR} "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
			'{COPY} "%{wks.location}/PFF/vendor/entt/LICENSE.txt" "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
			'{COPYDIR} "%{wks.location}/PFF/vendor/entt/include/entt" "%{wks.location}/bin/' .. outputs .. '/vendor/entt"',
	
			'{MKDIR} "%{wks.location}/bin/' .. outputs .. '/vendor/glm"',
			'{COPYDIR} "%{wks.location}/PFF/vendor/glm/glm" "%{wks.location}/bin/' .. outputs .. '/vendor/glm/glm"',
	
			-- copy GLFW
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/glfw",
			'{COPY} "%{wks.location}/PFF/vendor/glfw/build/src/libglfw3.a" "%{wks.location}/bin/' .. outputs .. '/vendor/glfw"',
			'{COPY} "%{wks.location}/PFF/vendor/glfw/LICENSE.md" "%{wks.location}/bin/' .. outputs .. '/vendor/glfw"',

			-- copy premake (needed for engine projects)
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"{COPY} %{wks.location}/premake5 %{wks.location}/bin/" .. outputs .. "/vendor/premake",
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
