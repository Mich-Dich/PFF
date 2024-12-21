
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
		
		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		postbuildcommands
		{
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/vulkan-glslc",
			"{COPY} %{wks.location}/PFF/vendor/vulkan-glslc %{wks.location}/bin/" .. outputs  .. "/vendor/vulkan-glslc",
			
			-- copy premake exe (needed for engine projects)
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"{COPY} %{wks.location}/vendor/premake %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			
			-- postbuildcommands { table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/defaults", "PFF/assets"})), }
			table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/defaults", "PFF/assets"})),
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
