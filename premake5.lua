workspace "Lypant"

	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	startproject "SandBox"

outputdir = "%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/"

project "Lypant"

	location "Lypant"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"LYPANT_PLATFORM_WINDOWS",
			"LYPANT_BUILD_DLL"
		}

		postbuildcommands
		{
			"{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[%{!sln.location}bin/" .. outputdir .. "SandBox/Lypant.dll]"
		}
		
		buildoptions { "/utf-8" }

	filter "configurations:Debug"

		defines "LYPANT_DEBUG"
		symbols "On"

	filter "configurations:Release"

		defines "LYPANT_RELEASE"
		optimize "On"


project "SandBox"

	links
	{
		"Lypant"
	}

	location "SandBox"
	kind "ConsoleApp"
	language "C++"

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
		"Lypant/vendor/spdlog/include"
	}

	filter "system:windows"
		
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines "LYPANT_PLATFORM_WINDOWS"

		buildoptions { "/utf-8" }

	filter "configurations:Debug"

		defines "LYPANT_DEBUG"
		symbols "On"

	filter "configurations:Release"

		defines "LYPANT_RELEASE"
		optimize "On"
