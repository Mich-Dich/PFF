
------------------------------------ CLIENT GAME ------------------------------------

project (client_project_name)
	location ("%{wks.location}/" .. client_project_name)
	kind "ConsoleApp"
	staticruntime "off"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")
	
	defines
	{
		"ENGINE_NAME=PFF",
		"PROJECT_NAME=%{client_project_name}",
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
		"%{wks.location}/PFF/src",

		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		
		"C:/VulkanSDK/1.3.250.1/Include",
	}

	links
	{
		"ImGui",
		"PFF",
		"PFF_editor",
	}

	filter "system:windows"
		-- cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		buildoptions "/MDd"
		defines "GAME_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GAME_RELEASE"
		runtime "Release"
		optimize "on"
			
	filter "configurations:RelWithDebInfo"
		buildoptions "/MD"
		defines "PFF_RELEASE_WITH_DEBUG_INFO"
		runtime "Release"
		symbols "on"
		optimize "speed"




		
-- filter "configurations:Game only in debug-mode"
-- 	buildoptions "/MDd"
-- 	defines "PFF_GAME_ONLY_DEBUG"
-- 	runtime "Debug"
-- 	symbols "on"

-- filter "configurations:Game only in release-mode"
-- 	buildoptions "/MD"
-- 	defines "PFF_GAME_ONLY_RELEASE_WITH_DEBUG_INFO"
-- 	runtime "Release"
-- 	symbols "on"
-- 	optimize "speed"

-- filter "configurations:Editor in debug-mode"
-- 	buildoptions "/MDd"
-- 	defines "PFF_EDITOR_DEBUG"
-- 	runtime "Debug"
-- 	symbols "on"

-- filter "configurations:Editor only in release-mode"
-- 	buildoptions "/MD"
-- 	defines "PFF_EDITOR_RELEASE_WITH_DEBUG_INFO"
-- 	runtime "Release"
-- 	symbols "on"
-- 	optimize "speed"
