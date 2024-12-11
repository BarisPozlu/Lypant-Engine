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
		int GetUniformLocation(const std::string& name);

		void SetMatrix4FloatUniform(const std::string& name, float* value);
		void SetVec4FloatUniform(const std::string& name, float* value);
	private:
		void CreateShader(const std::string& vsPath, const std::string& fsPath);
		void GetShaderSource(const std::string& vsPath, const std::string& fsPath, std::string& outVertexShaderSrc, std::string& outFragmentShaderSrc);
	private:
		uint32_t m_Program;
	};
}
