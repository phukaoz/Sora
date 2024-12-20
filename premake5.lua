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
	include "vendor/premake"
	include "Sora/vendor/glfw"
	include "Sora/vendor/Glad"
	include "Sora/vendor/imgui"
	include "Sora/vendor/Yaml-cpp"
	include "Sora/vendor/box2d"
group ""

group "Core"
	include "Sora"
	include "Sora-ScriptCore"
group ""

group "Tools"
	include "SoraEditor"
group ""

group "Misc"
	include "Sandbox"
group ""