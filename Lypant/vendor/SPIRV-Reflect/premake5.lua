project "Spirv-Reflect"

kind "StaticLib"
language "C"
staticruntime "off"

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

files
{
	"include/spirv/unified1/spirv.h",
	"spirv_reflect.h",
	"spirv_reflect.c"
}

filter "system:windows"

	systemversion "latest"
	
filter "configurations:Debug"

	runtime "Debug"
	symbols "on"

filter "configurations:Release"

	runtime "Release"
	optimize "on"
