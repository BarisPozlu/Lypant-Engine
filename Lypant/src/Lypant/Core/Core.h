#pragma once

// Supporting windows only for now.

#ifdef LYPANT_PLATFORM_WINDOWS
	#ifdef LYPANT_DYNAMIC_LINK
		#ifdef LYPANT_BUILD
			#define LYPANT_API __declspec(dllexport)
		#else
			#define LYPANT_API __declspec(dllimport)
			#define IMGUI_API __declspec(dllimport)
		#endif
	#else
		#define LYPANT_API
	#endif
	#ifdef LYPANT_DEBUG
		#ifdef LYPANT_BUILD
			#define LY_CORE_ASSERT(x, ...) { if(!(x)) { LY_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
			#define LY_CORE_VERIFY(x, ...) { if(!(x)) { LY_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#endif
		#define LY_ASSERT(x, ...) { if(!(x)) { LY_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define LY_VERIFY(x, ...) { if(!(x)) { LY_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#else
		#ifdef LYPANT_BUILD
			#define LY_CORE_ASSERT(x, ...)
			#define LY_CORE_VERIFY(x, ...) x
		#endif
		#define LY_ASSERT(x, ...)
		#define LY_VERIFY(x, ...) x
	#endif

#else 
	#error Lypant only supports windows for now.
#endif

#define LY_BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)
