include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Sora"
	architecture "x86_64"
	startproject "SoraEditor"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependecies"
	include "vendor/Premake"
	include "Sora/vendor/Glfw"
	include "Sora/vendor/Glad"
	include "Sora/vendor/Imgui"
	include "Sora/vendor/Yaml-cpp"
group ""

group "Core"
	include "Sora"
group ""

group "Tools"
	include "SoraEditor"
group ""

group "Misc"
	include "Sandbox"
group ""