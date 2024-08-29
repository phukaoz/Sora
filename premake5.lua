include "./vendor/premake/premake_customization/solution_items.lua"

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

IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/Sora/vendor/glfw/include"
IncludeDir["Glad"] = "%{wks.location}/Sora/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Sora/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Sora/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Sora/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Sora/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Sora/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Sora/vendor/ImGuizmo"

group "Dependecies"
	include "vendor/premake"
	include "Sora/vendor/glfw"
	include "Sora/vendor/Glad"
	include "Sora/vendor/imgui"
	include "Sora/vendor/yaml-cpp"
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