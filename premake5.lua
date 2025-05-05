
include "dependencies.lua"

workspace "PFF"
	platforms "x64"
	startproject "PFF_editor"

	configurations
	{
		"Debug",
		"RelWithDebInfo",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	local project_path = os.getcwd()
	defines {
		"ENGINE_INSTALL_DIR=\"" .. project_path .. "/bin/" .. outputs .. "\"",
		"PFF_OUTPUTS=\"" .. outputs .. "\"",
		"PFF_USING_EDITOR",							-- TODO: PFF_USING_EDITOR should not be defined in the packaged builds of game
	}

	-- local RESET = "\27[0m"
	-- local GREEN = "\27[32m"

    -- Set a custom build message
    -- buildmessage(GREEN .. " ?????????????????????????? Building %{prj.name} (%{cfg.buildcfg} - %{cfg.system} - %{cfg.architecture})")

	if os.target() == "linux" then
		print("---------- target platform is linux => manually compile GLFW ----------")
		os.execute("cmake -S ./PFF/vendor/glfw -B ./PFF/vendor/glfw/build")		-- manuel compilation
		os.execute("cmake --build ./PFF/vendor/glfw/build")						-- manuel compilation
		print("---------- Done compiling GLFW ----------")
	end

function copy_content_of_dir(outputs, dir_names)
	local commands = {}
	for _, dir_name in ipairs(dir_names) do
		local target_dir = "%{wks.location}/bin/" .. outputs .. "/" .. dir_name
		local source_dir = "%{wks.location}/" .. dir_name

		table.insert(commands, "{MKDIR} " .. target_dir)

		if os.target() == "windows" then
			table.insert(commands, "{COPY} " .. source_dir .. " " .. target_dir)
		elseif os.target() == "linux" then
			table.insert(commands, "{COPY} " .. source_dir .. " " .. target_dir .. "/..")
		end
	end

	return commands
end

group "dependencies"
	include "PFF/vendor/fastgltf"
	include "PFF/vendor/imgui"
	if os.target() == "windows" then
		include "PFF/vendor/glfw"
	end
group ""

group "Engine"
	include "PFF"
group ""

group "Tools"
	include "PFF_editor"
	include "PFF_helper"
group ""
