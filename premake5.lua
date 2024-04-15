
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
	include "PFF/vendor/imgui"
	include "PFF/vendor/glfw"
group ""


include "PFF"
include "PFF_editor"
include "Sandbox"