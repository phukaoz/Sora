
project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"src/**.h",
		"src/**.cpp",
	}
	
	includedirs{
		"%{wks.location}/Yuki/vendor/spdlog/include",
		"%{wks.location}/Yuki/src",
		"%{wks.location}/Yuki/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links{
		"Yuki"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "YUKI_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "YUKI_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "YUKI_DIST"
		runtime "Release"
		optimize "on"