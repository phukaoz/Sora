include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Yuki"
	architecture "x86_64"
	startproject "Alya"
	
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
IncludeDir["GLFW"] = "%{wks.location}/Yuki/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Yuki/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Yuki/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Yuki/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Yuki/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Yuki/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Yuki/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Yuki/vendor/ImGuizmo"

group "Dependecies"
	include "vendor/premake"
	include "Yuki/vendor/GLFW"
	include "Yuki/vendor/Glad"
	include "Yuki/vendor/imgui"
	include "Yuki/vendor/yaml-cpp"
group ""

group "Core"
	include "Yuki"
group ""

group "Tools"
	include "Alya"
group ""

group "Misc"
	include "Sandbox"
group ""