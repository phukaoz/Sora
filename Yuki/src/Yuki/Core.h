#pragma once

#ifdef YUKI_PLATFORM_WINDOWS
#if YUKI_DYNAMIC_LINK
	#ifdef YUKI_BUILD_DLL
		#define YUKI_API __declspec(dllexport)
	#else
		#define YUKI_API __declspec(dllimport)
	#endif
#else
	#define YUKI_API
#endif
#else
	#error Yuki only support Windows!
#endif

#ifdef YUKI_DEBUG
	#define YUKI_ENABLE_ASSERTS
#endif

#ifdef YUKI_ENABLE_ASSERTS
	#define YUKI_ASSERT(x, ...) {if(!(x)) { YUKI_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define YUKI_CORE_ASSERT(x, ...) {if(!(x)) { YUKI_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define YUKI_ASSERT(x, ...)
	#define YUKI_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define YUKI_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
