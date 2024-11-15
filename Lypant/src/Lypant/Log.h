#pragma once

#include "lypch.h"
#include "Core.h"
#include "spdlog/spdlog.h"

namespace lypant
{
	class LYPANT_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_AppLogger;
	};
}


#ifdef LYPANT_DEBUG

	#ifdef LYPANT_BUILD_DLL

		#define LY_CORE_TRACE(...)        ::lypant::Log::GetCoreLogger()->trace(__VA_ARGS__)
		#define LY_CORE_INFO(...)         ::lypant::Log::GetCoreLogger()->info(__VA_ARGS__)
		#define LY_CORE_WARNING(...)      ::lypant::Log::GetCoreLogger()->warn(__VA_ARGS__)
		#define LY_CORE_ERROR(...)        ::lypant::Log::GetCoreLogger()->error(__VA_ARGS__)
		#define LY_CORE_CRITICAL(...)     ::lypant::Log::GetCoreLogger()->critical(__VA_ARGS__)

	#endif

	#define LY_TRACE(...)        ::lypant::Log::GetAppLogger()->trace(__VA_ARGS__)
	#define LY_INFO(...)         ::lypant::Log::GetAppLogger()->info(__VA_ARGS__)
	#define LY_WARNING(...)      ::lypant::Log::GetAppLogger()->warn(__VA_ARGS__)
	#define LY_ERROR(...)        ::lypant::Log::GetAppLogger()->error(__VA_ARGS__)
	#define LY_CRITICAL(...)     ::lypant::Log::GetAppLogger()->critical(__VA_ARGS__)

#else

	#ifdef LYPANT_BUILD_DLL

		#define LY_CORE_TRACE(...)
		#define LY_CORE_INFO(...)
		#define LY_CORE_WARNING(...)
		#define LY_CORE_ERROR(...)
		#define LY_CORE_CRITICAL(...)

	#endif

	#define LY_TRACE(...)   
	#define LY_INFO(...)    
	#define LY_WARNING(...) 
	#define LY_ERROR(...)   
	#define LY_CRITICAL(...)

#endif 
