#pragma once

#include "lypch.h"

namespace lypant
{
	class Shader
	{
	public:
		Shader(const std::string& path);
		~Shader();
		void Bind() const;
		int GetUniformLocation(const std::string& name) const;

		void SetUniformMatrix4Float(const std::string& name, float* value) const;
		void SetUniformVec4Float(const std::string& name, float* value) const;
		void SetUniformInt(const std::string& name, int value) const;
	private:
		uint32_t m_Program;
	};
}
