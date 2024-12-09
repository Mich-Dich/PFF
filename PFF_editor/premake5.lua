
project "PFF_editor"
	location "%{wks.location}/PFF_editor"
	kind "ConsoleApp"
	staticruntime "off"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	pchheader "util/pch_editor.h"
	pchsource "src/util/pch_editor.cpp"

	-- glslc = "%{wks.location}/PFF/vendor/vulkan-glslc/glslc.exe"

	defines
	{
		"ENGINE_NAME=PFF",
		"PFF_INSIDE_EDITOR",
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
		"content",
		"%{wks.location}/PFF/src",

		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		"%{IncludeDir.fastgltf}",
		"%{IncludeDir.tinyobjloader}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGuizmo}",

		"C:/VulkanSDK/1.3.250.1/Include",
	}

	links
	{
		"ImGui",
		"fastgltf",
		"PFF",
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
		
		files					-- Include the icon resource only for Windows
		{
			"../metadata/app_icon.rc",
		}

        postbuildcommands { table.unpack(copy_content_of_dir(outputs, {"PFF_editor/shaders", "PFF_editor/defaults", "PFF_editor/assets"})), }
		
	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "PFF_EDITOR_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:RelWithDebInfo"
		defines "PFF_EDITOR_RELEASE_WITH_DEBUG_INFO"
		buildoptions "/MD"
		runtime "Release"
		symbols "on"
		optimize "On"

	filter "configurations:Release"
		defines "PFF_EDITOR_RELEASE"
		buildoptions "/MDd"
		runtime "Release"
		optimize "on"
		