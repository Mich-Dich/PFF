
project "PFF_editor"
		location "%{wks.location}/PFF_editor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"
	
		targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
		objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")
	
		pchheader "util/pch_editor.h"
		pchsource "src/util/pch_editor.cpp"
	
		glslc = "%{wks.location}/PFF/vendor/vulkan-glslc/glslc.exe"
	
		defines
		{
			"ENGINE_NAME=PFF",
			"PROJECT_NAME=PFF_editor",
			"PFF_INSIDE_EDITOR",
		}
	
		files
		{
			"src/**.h",
			"src/**.cpp",
			"**.vert",
			"**.frag",
		}
	
		includedirs
		{
            "src",
			"assets",
			"shaders",

			"%{wks.location}/PFF/vendor/glm",
			"%{wks.location}/PFF/src",
			"%{wks.location}/PFF/vendor/glm",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.ImGui}/backends/",
            
            "C:/VulkanSDK/1.3.250.1/Include",
		}
	
		links
		{
			"ImGui",
			"PFF",
		}
	
		filter "system:windows"
			systemversion "latest"
	
			defines
			{
				"PFF_PLATFORM_WINDOWS",
			}
	
			
	
		filter "configurations:Debug"
			buildoptions "/MDd"
			defines "EDITOR_DEBUG"
			runtime "Debug"
			symbols "on"
	
		filter "configurations:RelWithDebInfo"
			defines "PFF_RELEASE_WITH_DEBUG_INFO"
			runtime "Release"
			symbols "on"
			optimize "speed"

		filter "configurations:Release"
			defines "EDITOR_RELEASE"
			runtime "Release"
			optimize "on"
			