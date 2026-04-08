#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Image.h"
#include "Lypant/Util/Images.h"

namespace lypant
{
	struct MaterialTextures
	{
	public:
		inline std::array<std::shared_ptr<Image>, 6> Get() const { return { AlbedoMap, ORMMap, AmbientOcclusionMap, RoughnessMap, MetallicMap, NormalMap }; }
	public:
		std::shared_ptr<Image> AlbedoMap = util::Images::GetWhite4Channel1x1();
		std::shared_ptr<Image> ORMMap = util::Images::GetWhite4Channel1x1();
		std::shared_ptr<Image> AmbientOcclusionMap = util::Images::GetWhite1Channel1x1();
		std::shared_ptr<Image> RoughnessMap = util::Images::GetWhite1Channel1x1();
		std::shared_ptr<Image> MetallicMap = util::Images::GetWhite1Channel1x1();
		// TODO: this default normal map is bad, it does not cause issues for now since if useNormalMap flag is set then we know we already
		// have non-default normal map otherwise this map is simply not used
		std::shared_ptr<Image> NormalMap = util::Images::GetWhite4Channel1x1();
	};

	struct alignas(16) MaterialConstants
	{
		glm::vec3 Albedo = glm::vec3(1.0f);
		float Roughness = 1.0f;
		float Metallic = 1.0f;
		int UseCombinedORM = false;
		int UseNormalMap = false;
	};

	struct MaterialData
	{
		MaterialTextures Textures;
		MaterialConstants Constants;
	};

	class Material
	{
	public:
		static std::shared_ptr<Material> Create(const MaterialData& data);
		virtual ~Material() = default;
		virtual void UpdateMaterial() = 0;
		virtual const MaterialData& GetData() const = 0;
	};
}
