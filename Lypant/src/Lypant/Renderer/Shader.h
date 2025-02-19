#pragma once

#include "lypch.h"

// this is not supposed to be included in the application in the future when the engine matures.
// when that happens make the asserts core again.

namespace lypant
{
	enum class ShaderDataType
	{
		None, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Sampler2D
	};

	inline uint32_t GetSizeFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:		LY_ASSERT(false, "Shader data type is none!"); return 0;
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
		}

		LY_ASSERT(false, "Invalid Shader data type!");
		return 0;
	}

	inline uint32_t GetCountFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:		LY_ASSERT(false, "Shader data type is none!"); return 0;
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4;
			case ShaderDataType::Int:		return 1;
		}

		LY_ASSERT(false, "Invalid Shader data type!");
		return 0;
	}

	class Shader
	{
	public:
		inline static std::shared_ptr<Shader> Load(const std::string& path)
		{
			const auto it = s_Cache.find(path);
			if (it != s_Cache.end())
			{
				return std::shared_ptr<Shader>(it->second);
			}

			std::shared_ptr<Shader> shader = std::make_shared<Shader>(path);
			s_Cache[path] = std::weak_ptr<Shader>(shader);
			return shader;
		}
	public:
		Shader(const std::string& path);
		~Shader();

		inline uint32_t GetID() const { return m_Program; }
		inline const std::unordered_map<std::string, ShaderDataType>& GetUniformNamesToTypesMap() const { return m_UniformNamesToTypesMap; }

		void PopulateUniformTypes();
		void Bind() const;
		int GetUniformLocation(const std::string& name) const;

		int GetUniformValueInt(const std::string& name) const;

		void SetUniformMatrix4Float(const std::string& name, float* value) const;
		void SetUniformMatrix3Float(const std::string& name, float* value) const;

		void SetUniformVec4Float(const std::string& name, float* value) const;
		void SetUniformVec3Float(const std::string& name, float* value) const;

		void SetUniformInt(const std::string& name, int value) const;
	private:
		inline static std::unordered_map<std::string, std::weak_ptr<Shader>> s_Cache;
	private:
		std::unordered_map<std::string, ShaderDataType> m_UniformNamesToTypesMap;
		std::string m_Path;
		uint32_t m_Program;
	};
}
