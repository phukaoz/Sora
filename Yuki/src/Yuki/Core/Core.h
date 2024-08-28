#pragma once

#include <memory>

#include "Yuki/Core/PlatformDetection.h"

#ifdef YUKI_DEBUG
	#if defined(YUKI_PLATFORM_WINDOWS)
		#define YUKI_DEBUGBREAK() __debugbreak()
	#elif defined(YUKI_PLATFORM_LINUX)
		#include <signal.h>
		#define YUKI_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define YUKI_ENABLE_ASSERTS
#else
	#define YUKI_DEBUGBREAK()
#endif

#ifdef YUKI_ENABLE_ASSERTS
	#define YUKI_ASSERT(x, ...) {if(!(x)) { YUKI_ERROR("Assertion Failed: {0}", __VA_ARGS__); YUKI_DEBUGBREAK(); } }
	#define YUKI_CORE_ASSERT(x, ...) {if(!(x)) { YUKI_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); YUKI_DEBUGBREAK(); } }
#else
	#define YUKI_ASSERT(x, ...)
	#define YUKI_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define YUKI_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Yuki {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
