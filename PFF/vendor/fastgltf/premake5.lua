project "fastgltf"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"

	targetdir ("bin/" .. outputs .. "/%{prj.name}")
	objdir ("bin-int/" .. outputs .. "/%{prj.name}")

	files
	{
		"include/fastgltf/base64.hpp",
		"include/fastgltf/core.hpp",
		"include/fastgltf/glm_element_traits.hpp",
		"include/fastgltf/tools.hpp",
		"include/fastgltf/types.hpp",
		"include/fastgltf/util.hpp",
		"src/fastgltf.cpp",
		"src/base64.cpp",

		"deps/simdjson/simdjson.h",
		"deps/simdjson/simdjson.cpp",
	}

	includedirs
	{
		"deps/simdjson",
		"include",
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		pic "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:RelWithDebInfo"
		runtime "Release"
		symbols "on"
		optimize "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
