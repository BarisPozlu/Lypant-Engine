#pragma once

// Supporting windows only for now.

#ifdef LYPANT_PLATFORM_WINDOWS
	#ifdef LYPANT_BUILD_DLL
		#define LYPANT_API __declspec(dllexport)
	#else
		#define LYPANT_API __declspec(dllimport)
	#endif
#endif
