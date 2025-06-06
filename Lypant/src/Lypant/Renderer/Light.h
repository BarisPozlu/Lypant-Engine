#pragma once

#include <glm/glm.hpp>
#include "Lypant/Scene/Components.h"

namespace lypant
{
	enum LightType
	{
		LightTypeUnknown, LightTypePoint, LightTypeSpot, LightTypeDirectional
	};

	struct Light
	{
	public:
		Light(const glm::vec3& color) : Color(color, 0)
		{
			Type = LightTypeUnknown;
		}
		inline LightType GetType() const { return Type; }
	public:
		LightType Type;
		glm::vec4 Color;
	};

	struct PointLight : public Light
	{
	public:
		PointLight(const PointLightComponent& component)
			: Light(component.Color), Position(component.Position), Linear(component.Linear), Quadratic(component.Quadratic)
		{
			Type = LightTypePoint;
		}
		PointLight(const glm::vec3& color, const glm::vec3& position, float linear = 0.0f, float quadratic = 1.0f)
			: Light(color), Position(position), Linear(linear), Quadratic(quadratic)
		{
			Type = LightTypePoint;
		}
	public:
		glm::vec3 Position;
		float Linear;
		float Quadratic;
		// 12 bytes padding needed for gpu.
		float Padding;
		float Padding2;
		float Padding3;
	};

	struct SpotLight : public Light
	{
	public:
		SpotLight(const SpotLightComponent& component)
			: Light(component.Color), Position(component.Position), Direction(component.Direction), OuterCutOff(component.OuterCutOff), InnerCutOff(component.InnerCutOff)
		{
			Type = LightTypeSpot;
		}
		SpotLight(const glm::vec3& color, const glm::vec3& position, const glm::vec3& direction, float outerCutOff = 17.5, float innerCutOff = 12.5)
			: Light(color), Position(position, 0), Direction(glm::normalize(direction)), OuterCutOff(glm::cos(glm::radians(outerCutOff))), InnerCutOff(glm::cos(glm::radians(innerCutOff)))
		{
			Type = LightTypeSpot;
		}

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::normalize(direction);
		}
	public:
		glm::vec4 Position;
		glm::vec3 Direction;
		float OuterCutOff;
		float InnerCutOff;
		// 12 bytes padding needed for gpu.
		float Padding;
		float Padding2;
		float Padding3;
	};

	struct DirectionalLight : public Light
	{
	public:
		DirectionalLight(const DirectionalLightComponent& component)
			: Light(component.Color), Direction(component.Direction, 1.0)
		{
			Type = LightTypeDirectional;
		}
		DirectionalLight(const glm::vec3& color, const glm::vec3& direction)
			: Light(color), Direction(glm::normalize(direction), 0)
		{
			Type = LightTypeDirectional;
		}

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::vec4(glm::normalize(direction), 0);
		}
	public:
		glm::vec4 Direction;
	};
}
