#pragma once

// TODO: this is to be updated

#include "lypch.h"

namespace lypant
{
	class Shader
	{
	public:
		Shader(const std::string& vsPath, const std::string& fsPath);
		~Shader();
		void Bind() const;
		int GetUniformLocation(const std::string& name) const;

		void SetMatrix4FloatUniform(const std::string& name, float* value) const;
		void SetVec4FloatUniform(const std::string& name, float* value) const;
	private:
		void CreateShader(const std::string& vsPath, const std::string& fsPath);
		void GetShaderSource(const std::string& vsPath, const std::string& fsPath, std::string& outVertexShaderSrc, std::string& outFragmentShaderSrc) const;
	private:
		uint32_t m_Program;
	};
}
