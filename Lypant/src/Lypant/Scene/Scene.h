#pragma once

#include <vector>
#include <entt.hpp>

namespace lypant
{
	class Entity;
	class PerspectiveCamera;
	class Skybox;

	struct PointLightComponent;
	struct SpotLightComponent;
	struct DirectionalLightComponent;

	class Scene
	{
	public:
		Scene();
		~Scene();
		// entities start with a transform component attached to them but is allowed to be removed
		Entity CreateEntity();
		// entities do not have parent-child relationship yet, all entities created here have transform and mesh components attached to them
		std::vector<Entity> LoadModel(const std::string& path, bool loadMaterials = true, bool flipUVs = true);
		void Tick(float deltaTime, const std::shared_ptr<PerspectiveCamera>& camera, const std::shared_ptr<Skybox>& skybox);
	private:
		void UpdateSceneData(const std::shared_ptr<PerspectiveCamera>& camera, const std::shared_ptr<Skybox>& skybox);
	private:
		struct SceneData
		{
			PointLightComponent* PointLightComponents = nullptr;
			SpotLightComponent* SpotLightComponents = nullptr;
			DirectionalLightComponent* DirectionalLightComponents = nullptr;
			int NumberOfPointLights = 0;
			int NumberOfSpotLights = 0;
			int NumberOfDirectionalLights = 0;
			int NumberOfShadowCastingPointLights = 0;
			int NumberOfShadowCastingSpotLights = 0;
			int NumberOfShadowCastingDirectionalLights = 0;

			std::shared_ptr<PerspectiveCamera> Camera;
			std::shared_ptr<Skybox> Skybox;
		};

		SceneData m_SceneData;
		entt::registry m_Registry;
	private:
		friend class Entity;
		friend class Renderer;
	};
}
