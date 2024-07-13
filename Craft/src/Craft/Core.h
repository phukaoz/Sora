#pragma once

#ifdef CRAFT_PLATFORM_WINDOWS
	#ifdef CRAFT_BUILD_DLL
		#define CRAFT_API __declspec(dllexport)
	#else
		#define CRAFT_API __declspec(dllimport)
	#endif
#else
	#error Craft only support Windows!
#endif