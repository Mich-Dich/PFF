
project "PFF_editor"
	location "%{wks.location}/PFF_editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	pchheader "util/pch_editor.h"
	pchsource "src/util/pch_editor.cpp"

	-- glslc = "%{wks.location}/PFF/vendor/vulkan-glslc/glslc.exe"

	defines
	{
		"PFF_EDITOR",
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.embed",
	}

	includedirs
	{
		"src",
		"assets",
		"content",
		"%{wks.location}/PFF/src",
		"%{wks.location}/PFF/vendor",

		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends/",
		"%{IncludeDir.fastgltf}",
		"%{IncludeDir.tinyobjloader}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.VulkanSDK}",
	}

	links
	{
		"PFF",
		"PFF_helper",
		"ImGui",
		"fastgltf",
	}
	
	filter "system:windows"
		systemversion "latest"

		defines { "PFF_PLATFORM_WINDOWS" }
		files { "../metadata/app_icon.rc" }

        postbuildcommands 
		{
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",
			"{COPY} %{wks.location}/.github/wiki %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",
			
			table.unpack(copy_content_of_dir(outputs, {"PFF_editor/shaders", "PFF_editor/defaults", "PFF_editor/assets"})),
		}
		
	filter "configurations:Debug"
		defines "PFF_EDITOR_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:RelWithDebInfo"
		defines "PFF_EDITOR_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "on"

	filter "configurations:Release"
		defines "PFF_EDITOR_RELEASE"
		runtime "Release"
		optimize "on"
