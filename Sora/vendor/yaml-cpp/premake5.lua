project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	
	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files{
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}
	
	includedirs{
		"include"
	}
	
	defines{
		"YAML_CPP_STATIC_DEFINE"
	}
	
	filter "system:windows"
		systemversion "latest"
		
	filter "system:linux"
		pic "on"
		systemversion "latest"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "on"