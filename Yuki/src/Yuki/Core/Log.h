#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Yuki {

	class Log 
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros ///////////////////////////////////////////////////////////////////////
#define YUKI_CORE_TRACE(...)	Yuki::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define YUKI_CORE_INFO(...)		Yuki::Log::GetCoreLogger()->info(__VA_ARGS__)
#define YUKI_CORE_WARN(...)		Yuki::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define YUKI_CORE_ERROR(...)	Yuki::Log::GetCoreLogger()->error(__VA_ARGS__)
#define YUKI_CORE_FATAL(...)	Yuki::Log::GetCoreLogger()->fatal(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////////////////////

// Client log macros /////////////////////////////////////////////////////////////////////
#define YUKI_TRACE(...)			Yuki::Log::GetClientLogger()->trace(__VA_ARGS__)
#define YUKI_INFO(...)			Yuki::Log::GetClientLogger()->info(__VA_ARGS__)
#define YUKI_WARN(...)			Yuki::Log::GetClientLogger()->warn(__VA_ARGS__)
#define YUKI_ERROR(...)			Yuki::Log::GetClientLogger()->error(__VA_ARGS__)
#define YUKI_FATAL(...)			Yuki::Log::GetClientLogger()->fatal(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////////////////////