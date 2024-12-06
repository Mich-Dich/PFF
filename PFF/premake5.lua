
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
		
		-- links
		-- {
		-- 	"MSVCRT",
		-- 	"MSVCPRT"
		-- }

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
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/PFF_editor",
			"{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputs  .. "/PFF_editor",
			
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/vulkan-glslc",
			"{COPY} %{wks.location}/PFF/vendor/vulkan-glslc %{wks.location}/bin/" .. outputs  .. "/vendor/vulkan-glslc",
			
			-- copy premake exe
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			"{COPY} %{wks.location}/vendor/premake %{wks.location}/bin/" .. outputs .. "/vendor/premake",
			
			postbuildcommands { table.unpack(copy_content_of_dir(outputs, {"PFF/shaders", "PFF/defaults", "PFF/assets"})), }
		}

	filter "configurations:Debug"
		defines "PFF_DEBUG"
		buildoptions "/MDd"
		runtime "Debug"
		symbols "on"

	filter "configurations:RelWithDebInfo"
		defines "PFF_RELEASE_WITH_DEBUG_INFO"
		buildoptions "/MD"
        runtime "Release"
		symbols "on"
		optimize "On"

	filter "configurations:Release"
		defines "PFF_RELEASE"
		buildoptions "/MD"
		runtime "Release"
		symbols "off"
		optimize "Full"
		