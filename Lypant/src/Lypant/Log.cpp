#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace lypant
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_AppLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("Lypant");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_AppLogger = spdlog::stdout_color_mt("Application");
		s_AppLogger->set_level(spdlog::level::trace);
	}
}
