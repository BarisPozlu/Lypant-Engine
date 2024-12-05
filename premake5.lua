workspace "Lypant"

	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	startproject "Sandbox"

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/"

include "Lypant/vendor/GLFW"
include "Lypant/vendor/Glad"
include "Lypant/vendor/ImGui"

project "Lypant"

	location "Lypant"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%{prj.name}")

	pchheader "lypch.h"
	pchsource "%{prj.name}/src/lypch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/GLFW/include",
		"%{prj.name}/vendor/Glad/include",
		"%{prj.name}/vendor/ImGui",
		"%{prj.name}/vendor/glm"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		
		cppdialect "C++17"
		systemversion "latest"


		defines
		{
			"LYPANT_PLATFORM_WINDOWS",
			"LYPANT_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		prebuildcommands
		{
			"{MKDIR} %{!sln.location}bin/" .. outputdir .. "Sandbox/"
		}
		
		postbuildcommands
		{
			"{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[%{!sln.location}bin/" .. outputdir .. "Sandbox/Lypant.dll]"
		}
		
		buildoptions { "/utf-8" }

	filter "configurations:Debug"

		defines "LYPANT_DEBUG"
		runtime "debug"
		symbols "On"

	filter "configurations:Release"

		defines "LYPANT_RELEASE"
		runtime "release"
		optimize "On"


project "Sandbox"

	links
	{
		"Lypant"
	}

	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Lypant/src",
		"Lypant/vendor/spdlog/include",
		"Lypant/vendor/ImGui"
	}

	filter "system:windows"
		
		cppdialect "C++17"
		systemversion "latest"

		defines "LYPANT_PLATFORM_WINDOWS"

		buildoptions { "/utf-8" }

	filter "configurations:Debug"

		defines "LYPANT_DEBUG"
		runtime "debug"
		symbols "On"

	filter "configurations:Release"

		defines "LYPANT_RELEASE"
		runtime "release"
		optimize "On"
