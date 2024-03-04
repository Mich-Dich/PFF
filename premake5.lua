
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

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "dependencies"
	-- include "vendor/premake"
	include "PFF/vendor/imgui_new"
	include "PFF/vendor/glfw"
group ""


include "PFF"
include "PFF_editor"
include "Sandbox"