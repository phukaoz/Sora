workspace "Yuki"
	architecture "x64"
	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Yuki/vendor/GLFW/include"
IncludeDir["Glad"] = "Yuki/vendor/Glad/include"

include "Yuki/vendor/GLFW"
include "Yuki/vendor/Glad"

project "Yuki"
	location "Yuki"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Yukipch.h"
	pchsource "Yuki/src/Yukipch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
	}

	links{
		"GLFW",
		"Glad",
		"opengl32.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines{ 
			"YUKI_PLATFORM_WINDOWS",
			"YUKI_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

		postbuildcommands{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "YUKI_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "YUKI_RELEASE"
		symbols "on"

	filter "configurations:Debug"
		defines "YUKI_DIST"
		symbols "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs{
		"Yuki/vendor/spdlog/include",
		"Yuki/src",
	}

	links{
		"Yuki"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines{ 
			"YUKI_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "YUKI_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "YUKI_RELEASE"
		symbols "on"

	filter "configurations:Debug"
		defines "YUKI_DIST"
		symbols "on"