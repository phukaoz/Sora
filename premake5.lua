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
IncludeDir["ImGui"] = "Yuki/vendor/imgui"
IncludeDir["glm"] = "Yuki/vendor/glm"
IncludeDir["stb_image"] = "Yuki/vendor/stb_image"
IncludeDir["entt"] = "Yuki/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Yuki/vendor/yaml-cpp/include"

include "Yuki/vendor/GLFW"
include "Yuki/vendor/Glad"
include "Yuki/vendor/imgui"
include "Yuki/vendor/yaml-cpp"

project "Yuki"
	location "Yuki"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Yukipch.h"
	pchsource "Yuki/src/Yukipch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}
	
	defines{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}

	includedirs{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
	}

	links{
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"

		defines{ 
			"YUKI_PLATFORM_WINDOWS",
			"YUKI_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

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

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs{
		"Yuki/vendor/spdlog/include",
		"Yuki/src",
		"Yuki/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links{
		"Yuki"
	}

	filter "system:windows"
		systemversion "latest"

		defines{ 
			"YUKI_PLATFORM_WINDOWS",
		}

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

project "Alya"
	location "Alya"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}
	
	includedirs{
		"Yuki/vendor/spdlog/include",
		"Yuki/src",
		"Yuki/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links{
		"Yuki"
	}

	filter "system:windows"
		systemversion "latest"

		defines{ 
			"YUKI_PLATFORM_WINDOWS",
		}

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