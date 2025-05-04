
project "PFF_editor"
	location "%{wks.location}/PFF_editor"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	buildoptions { "-Wl,--verbose" }

	targetdir ("%{wks.location}/bin/" .. outputs  .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputs  .. "/%{prj.name}")

	pchheader "util/pch_editor.h"
	pchsource "src/util/pch_editor.cpp"

	-- glslc = "%{wks.location}/PFF/vendor/vulkan-glslc/glslc.exe"

	defines { "PFF_EDITOR" }

	files {
		"src/**.h",
		"src/**.cpp",
		"src/**.embed",
	}

	includedirs {
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
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.VulkanUtils}",
		"%{IncludeDir.assimp}",
	}

	links {
		"PFF",
		"PFF_helper",
		"ImGui",
		"fastgltf",
		"assimp",
	}

	libdirs {
		'%{wks.location}/%{vendor_path.assimp}/bin/' .. outputs .. '/assimp',
	}

	filter "system:windows"
		systemversion "latest"
		defines "PFF_PLATFORM_WINDOWS"

		files {
			"../metadata/app_icon.rc",
		}

	    links {
			-- "%{Library.Vulkan}",
        	"assimp",
		}
		
		libdirs {
			"%{IncludeDir.Vulkan}/Lib",
		}
        
		postbuildcommands
		{
			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",
			"{COPY} %{wks.location}/.github/wiki %{wks.location}/bin/" .. outputs .. "/PFF_editor/wiki",

			"{MKDIR} %{wks.location}/bin/" .. outputs .. "/PFF_editor/shaders",
			'{COPYDIR} "%{wks.location}/PFF_editor/shaders" "%{wks.location}/bin/' .. outputs .. '/PFF_editor/shaders"',
			'{COPYDIR} "%{wks.location}/PFF_editor/assets" "%{wks.location}/bin/' .. outputs .. '/PFF_editor/assets"',
			'{COPYDIR} "%{wks.location}/PFF_editor/defaults" "%{wks.location}/bin/' .. outputs .. '/PFF_editor/defaults"',
			'{COPYDIR} "%{wks.location}/PFF_editor/project_templates" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
		}


	filter "system:linux"
		pic "On"
		systemversion "latest"
		defines "PFF_PLATFORM_LINUX"
		
		includedirs {

			-- "/usr/include/vulkan",
			"%{IncludeDir.glfw}/include",
			"%{IncludeDir.assimp}",

			"/usr/include/x86_64-linux-gnu/qt5", 				-- Base Qt include path
			"/usr/include/x86_64-linux-gnu/qt5/QtCore",
			"/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
			"/usr/include/x86_64-linux-gnu/qt5/QtGui",
		}

		libdirs {

			"%{wks.location}/PFF/vendor/glfw/build/src",
			"%{wks.location}/PFF_editor/vendor/assimp/bin/" .. outputs .. "/assimp",
			"/usr/lib/x86_64-linux-gnu",

			"/usr/lib/x86_64-linux-gnu",  -- Default library path for system libraries
			"/usr/lib/x86_64-linux-gnu/qt5",
			-- "%{IncludeDir.Vulkan}/lib",
		}

		links {

			"%{wks.location}/PFF/vendor/glfw/build/src/glfw3",
			-- "glfw",
			"imgui",
			"vulkan",
			"assimp",
			-- "/home/mich/workspace/PFF/PFF_editor/vendor/assimp/bin/Debug-linux-x86_64/assimp/ibassimp.a",
			-- "/home/mich/workspace/PFF/PFF_editor/vendor/assimp/bin/Debug-linux-x86_64/assimp/ibassimp.so",

			"Qt5Core",
			"Qt5Widgets",
			"Qt5Gui",
		}
		
		postbuildcommands {
			
			'{COPYDIR} "%{wks.location}/.github/wiki" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
			'{COPYDIR} "%{wks.location}/PFF_editor/shaders" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
			'{COPYDIR} "%{wks.location}/PFF_editor/assets" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
			'{COPYDIR} "%{wks.location}/PFF_editor/defaults" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
			'{COPYDIR} "%{wks.location}/PFF_editor/project_templates" "%{wks.location}/bin/' .. outputs .. '/PFF_editor"',
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
