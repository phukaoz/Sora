#pragma once

#include <memory>

#include "Sora/Core/PlatformDetection.h"

#ifdef SORA_DEBUG
	#if defined(SORA_PLATFORM_WINDOWS)
		#define SORA_DEBUGBREAK() __debugbreak()
	#elif defined(SORA_PLATFORM_LINUX)
		#include <signal.h>
		#define SORA_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define SORA_ENABLE_ASSERTS
#else
	#define SORA_DEBUGBREAK()
#endif

#ifdef SORA_ENABLE_ASSERTS
	#define SORA_ASSERT(x, ...)	{if(!(x)) { SORA_ERROR("Assertion Failed: {0}", __VA_ARGS__); SORA_DEBUGBREAK(); } }
	#define SORA_CORE_ASSERT(x, ...) {if(!(x)) { SORA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SORA_DEBUGBREAK(); } }
#else
	#define SORA_ASSERT(x, ...)
	#define SORA_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define SORA_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Sora {

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
