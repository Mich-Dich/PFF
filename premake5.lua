
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

	-- TODO: PFF_USING_EDITOR should not be defined in the backaged builds of game
	defines
	{
		"PFF_USING_EDITOR",
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "dependencies"
	include "PFF/vendor/fastgltf"
	include "PFF/vendor/imgui"
	include "PFF/vendor/glfw"
group ""

function copy_content_of_dir(outputs, dir_names)
    local commands = {}
    for _, dir_name in ipairs(dir_names) do
        local target_dir = "%{wks.location}/bin/" .. outputs .. "/" .. dir_name
        local source_dir = "%{wks.location}/" .. dir_name

        print("Copying directory: " .. source_dir .. " to " .. target_dir)

        -- Create the target directory
        table.insert(commands, "{MKDIR} " .. target_dir)

        -- Use platform-specific copy commands
        if os.target() == "windows" then
            table.insert(commands, "{COPY} " .. source_dir .. " " .. target_dir)
        elseif os.target() == "linux" then
            table.insert(commands, "cp -r " .. source_dir .. " " .. target_dir)
        end
    end

    return commands
end


group "Engine"
	include "PFF"
group ""

group "Tools"
	include "PFF_editor"
	include "PFF_helper"
group ""
