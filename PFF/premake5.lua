
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
		"src/**.embeded",
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
		"imgui",
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
			
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/" .. client_project_name,
			"{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputs  .. "/" .. client_project_name,

			postbuildcommands { table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/defaults", "PFF/assets"})), }
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
		buildoptions "/MD"
		defines "PFF_RELEASE"
		runtime "Release"
		optimize "on"
