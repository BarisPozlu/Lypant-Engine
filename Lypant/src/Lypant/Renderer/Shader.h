#pragma once

#include <memory>
#include <string>

enum ShaderStageFlags
{
	ShaderStageFlagsNone = 0,
	ShaderStageFlagsVertex = 1,
	ShaderStageFlagsGeometry = 2,
	ShaderStageFlagsFragment = 4
};

namespace lypant
{
	class Shader
	{
	public:
		static std::shared_ptr<Shader> Create(const std::string& path);
		virtual ~Shader() = default;
	};
}