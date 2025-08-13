#pragma once

#include <memory>
#include <string>

namespace lypant
{
	class Shader
	{
	public:
		static std::shared_ptr<Shader> Create(const std::string& path);
		virtual ~Shader() = default;
	};
}