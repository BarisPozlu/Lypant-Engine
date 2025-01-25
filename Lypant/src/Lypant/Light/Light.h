#pragma once

#include "glm/glm.hpp"

namespace lypant
{
	enum LightType
	{
		LightTypePoint, LightTypeSpot, LightTypeDirectional
	};

	struct Light
	{
		Light(const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
			: Color(color, 0), Ambient(ambient, 0), Diffuse(diffuse, 0), Specular(specular, 0)
		{

		}

		virtual ~Light() = default;

		glm::vec4 Color;
		glm::vec4 Ambient;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
	};

	struct PointLight : public Light
	{
		PointLight(const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, float linear = 0.09f, float quadratic = 0.032)
			: Light(color, ambient, diffuse, specular), Position(position), Linear(linear), Quadratic(quadratic)
		{

		}

		glm::vec3 Position;
		float Linear;
		float Quadratic;
		// 12 bytes padding needed for gpu.
		float Padding;
		float Padding2;
		float Padding3;
	};

	struct DirectionalLight : public Light
	{
		DirectionalLight(const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& direction)
			: Light(color, ambient, diffuse, specular), Direction(glm::normalize(direction), 0)
		{

		}

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::vec4(glm::normalize(direction), 0);
		}
		
		glm::vec4 Direction;
	};

	struct SpotLight : public Light
	{
		SpotLight(const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& position, const glm::vec3& direction)
			: Light(color, ambient, diffuse, specular), Position(position, 0), Direction(glm::normalize(direction)), CutOff(45.0f)
		{

		}

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::normalize(direction);
		}

		glm::vec4 Position;
		glm::vec3 Direction;
		float CutOff;
	};
}