#pragma once

#ifdef YUKI_PLATFORM_WINDOWS
	#ifdef YUKI_BUILD_DLL
		#define YUKI_API __declspec(dllexport)
	#else
		#define YUKI_API __declspec(dllimport)
	#endif
#else
	#error Yuki only support Windows!
#endif

#define BIT(x) (1 << x)