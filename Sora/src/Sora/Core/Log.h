#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Sora {

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
#define SORA_CORE_TRACE(...)	Sora::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SORA_CORE_INFO(...)		Sora::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SORA_CORE_WARN(...)		Sora::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SORA_CORE_ERROR(...)	Sora::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SORA_CORE_FATAL(...)	Sora::Log::GetCoreLogger()->fatal(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////////////////////

// Client log macros /////////////////////////////////////////////////////////////////////
#define SORA_TRACE(...)			Sora::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SORA_INFO(...)			Sora::Log::GetClientLogger()->info(__VA_ARGS__)
#define SORA_WARN(...)			Sora::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SORA_ERROR(...)			Sora::Log::GetClientLogger()->error(__VA_ARGS__)
#define SORA_FATAL(...)			Sora::Log::GetClientLogger()->fatal(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////////////////////