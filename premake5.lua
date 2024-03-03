
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
	include "PFF/vendor/imgui_new"
	include "PFF/vendor/glfw"
	include "PFF/vendor/yaml-cpp"
group ""


include "PFF"
include "PFF_editor"
include "Sandbox"