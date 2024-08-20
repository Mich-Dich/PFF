
include "dependencies.lua"
client_project_name = "Sandbox"						-- This is the name of your project

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

		-- Debug print
		print("Copying directory: " .. source_dir .. " to " .. target_dir)

		-- Create target directory
		table.insert(commands, "{MKDIR} " .. target_dir)

		-- Add copy command
		table.insert(commands, "{COPY} " .. source_dir .. " " .. target_dir)
	end

	-- Debug print all commands
	-- print("Generated commands:")
	-- for _, cmd in ipairs(commands) do
	-- 	print(cmd)ls
	-- end

	return commands
end

include "PFF"
include "PFF_editor"
include "Sandbox"
