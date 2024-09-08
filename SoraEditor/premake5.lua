project "SoraEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}
	
	includedirs
	{
		"%{wks.location}/Sora/vendor/spdlog/include",
		"%{wks.location}/Sora/src",
		"%{wks.location}/Sora/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.box2d}"
	}

	links
	{
		"Sora"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SORA_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SORA_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SORA_DIST"
		runtime "Release"
		optimize "on"