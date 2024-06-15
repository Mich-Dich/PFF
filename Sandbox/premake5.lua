
------------------------------------ CLIENT GAME ------------------------------------

project (client_project_name)
	location (client_project_name)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputs  .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs  .. "/%{prj.name}")
	
	defines
	{
		"ENGINE_NAME=PFF",
		"PROJECT_NAME=%{client_project_name}",
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/**.vert",
		"%{prj.name}/**.frag",
	}
	
	includedirs
	{
		"%{IncludeDir.entt}",
		"%{IncludeDir.glm}",
		"PFF/src",
	}

	links
	{
		"PFF",
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"PFF_PLATFORM_WINDOWS",
		}

		postbuildcommands
		{
			'"%{glslc}" shaders/default.vert -o shaders/default.vert.spv',
			'"%{glslc}" shaders/default.frag -o shaders/default.frag.spv',
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
