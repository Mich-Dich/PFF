
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

			copy_content_of_dir(outputs, "PFF_editor/shaders"),
			copy_content_of_dir(outputs, "PFF_editor/assets"),
			copy_content_of_dir(outputs, "PFF_editor/defaults"),
			-- table.unpack(copy_content_of_dir({"PFF_editor/shaders", "PFF_editor/defaults", "PFF_editor/assets"})),
		}


	filter "system:linux"
		pic "On"
		systemversion "latest"
		defines { "PFF_PLATFORM_LINUX" }

		postbuildcommands
		{
			-- Create the directory if it doesn't exist
			"mkdir -p %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",
			"cp -r %{wks.location}/.github/wiki %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",

			-- Copy content of directories
			table.unpack(copy_content_of_dir(outputs, {"PFF_editor/shaders", "PFF_editor/defaults", "PFF_editor/assets"})),
		}

		includedirs
		{
			"/usr/include/vulkan",
			"%{IncludeDir.glfw}/include",

			"/usr/include/x86_64-linux-gnu/qt5", 				-- Base Qt include path
			"/usr/include/x86_64-linux-gnu/qt5/QtCore",
			"/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
			"/usr/include/x86_64-linux-gnu/qt5/QtGui",
		}

		libdirs
		{
			"%{wks.location}/PFF/vendor/glfw/build/src",
			
			"/usr/lib/x86_64-linux-gnu",  -- Default library path for system libraries
			"/usr/lib/x86_64-linux-gnu/qt5",
			-- "%{IncludeDir.VulkanSDK}/lib",
		}

		links
		{
			"%{wks.location}/PFF/vendor/glfw/build/src/glfw3",
			-- "glfw",
			"imgui",
			"vulkan",
			"vulkan",

			"Qt5Core",
			"Qt5Widgets",
			"Qt5Gui",
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
