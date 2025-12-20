#pragma once

#include <glm/glm.hpp>
#include <array>
#include "Lypant/Scene/Components.h"

namespace lypant
{
	struct CameraData
	{
		glm::mat4 ViewProjection;
		glm::mat4 View;
		glm::vec3 CameraPosition;
	};

	struct LightCountData
	{
		int PointLightCount;
		int SpotLightCount;
		int DirectionalLightCount;
	};

	// TODO: Remove the all the light data and simply use the components

	//struct PointLightData
	//{
	//	glm::vec3 Color;
	//	glm::vec3 Position;
	//	float Linear;
	//	float Quadratic;
	//	int ShadowMapIndex;
	//	bool CastShadows;
	//};

	//struct SpotLightData
	//{
	//	glm::vec3 Color;
	//	glm::vec3 Position;
	//	glm::vec3 Direction;
	//	float OuterCutOff;
	//	float InnerCutOff;
	//	int ShadowMapIndex;
	//	bool CastShadows;
	//};

	//struct DirectionalLightData
	//{
	//	glm::vec3 Color;
	//	glm::vec3 Direction;
	//	int ShadowMapIndex;
	//	bool CastShadows;
	//};

	struct alignas(64) EnvironmentBufferLayout
	{
		CameraData Camera;
		alignas(16) LightCountData LightCounts;
		alignas(16) std::array<PointLightComponent, 30> PointLights;
		alignas(16) std::array<SpotLightComponent, 30> SpotLights;
		alignas(16) std::array<DirectionalLightComponent, 30> DirectionalLights;
		
		inline static constexpr uint32_t s_BindingCount = 5;
		inline static int GetOffsetOfBinding(uint32_t binding)
		{
			switch (binding)
			{
				case 0: return offsetof(EnvironmentBufferLayout, Camera);
				case 1: return offsetof(EnvironmentBufferLayout, LightCounts);
				case 2: return offsetof(EnvironmentBufferLayout, PointLights);
				case 3: return offsetof(EnvironmentBufferLayout, SpotLights);
				case 4: return offsetof(EnvironmentBufferLayout, DirectionalLights);
			}

			LY_CORE_ASSERT(false, "Binding does not exist");
			return -1;
		}
		inline static int GetRangeOfBinding(uint32_t binding)
		{
			switch (binding)
			{
				case 0: return sizeof(Camera);
				case 1: return sizeof(LightCounts);
				case 2: return sizeof(PointLights);
				case 3: return sizeof(SpotLights);
				case 4: return sizeof(DirectionalLights);
			}

			LY_CORE_ASSERT(false, "Binding does not exist");
			return -1;
		}
	};
}
