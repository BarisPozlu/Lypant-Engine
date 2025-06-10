#include "lypch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "BehaviorComponent.h"
#include "Lypant/Renderer/Renderer.h"
#include "Lypant/Renderer/Texture.h"

namespace lypant
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
		m_Registry.view<BehaviorComponent>().each([](entt::entity entity, BehaviorComponent& component)
			{
				component.DestroyInstanceFunction(&component);
			});
	}

	Entity Scene::CreateEntity()
	{
		Entity entity(m_Registry.create(), this);
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	std::vector<Entity> Scene::LoadModel(const std::string& path, bool loadMaterials, bool flipUVs)
	{
		Model model(path, loadMaterials, flipUVs);
		std::vector<Entity> entities;
		entities.reserve(model.GetMeshes().size());

		for (const Mesh& mesh : model.GetMeshes())
		{
			Entity entity = CreateEntity();
			entity.AddComponent<MeshComponent>(mesh.GetVertexArray(), mesh.GetMaterial());
			entities.push_back(entity);
		}

		return entities;
	}

	void Scene::Tick(float deltaTime, const std::shared_ptr<PerspectiveCamera>& camera)
	{
		UpdateSceneData(camera);

		// Update Components
		m_Registry.view<BehaviorComponent>().each([this, deltaTime](entt::entity entityHandle, BehaviorComponent& component)
			{
				if (!component.Instance)
				{
					component.Instance = component.InstantiateFunction();
					component.Instance->m_Entity = Entity(entityHandle, this);
					component.Instance->BeginScene();
				}
				component.Instance->Tick(deltaTime);
			});

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		// Shadow Pass
		// We have a shadow pass for each light type
	
		// Only one directional light can cast shadows
		if (m_SceneData.NumberOfShadowCastingDirectionalLights > 0)
		{
			Renderer::BeginShadowPass(m_SceneData, LightTypeDirectional);

			for (entt::entity entity : group)
			{
				auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
				Renderer::SubmitForShadowPass(mesh.MeshData, transform, LightTypeDirectional, m_SceneData.NumberOfShadowCastingDirectionalLights);
			}
		}

		// At most 8 spot lights can cast shadows
		if (m_SceneData.NumberOfShadowCastingSpotLights > 0)
		{
			Renderer::BeginShadowPass(m_SceneData, LightTypeSpot);

			for (entt::entity entity : group)
			{
				auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
				Renderer::SubmitForShadowPass(mesh.MeshData, transform, LightTypeSpot, m_SceneData.NumberOfShadowCastingSpotLights);
			}
		}

		// At most 8 point lights can cast shadows
		if (m_SceneData.NumberOfShadowCastingPointLights > 0)
		{
			Renderer::BeginShadowPass(m_SceneData, LightTypePoint);

			for (entt::entity entity : group)
			{
				auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
				Renderer::SubmitForShadowPass(mesh.MeshData, transform, LightTypePoint, m_SceneData.NumberOfShadowCastingPointLights);
			}
		}

		Renderer::EndShadowPass();
		
		// Lighting Pass

		Renderer::BeginScene(m_SceneData);

		for (entt::entity entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			Renderer::Submit(mesh.MeshData, transform);
		}

		Renderer::EndScene();
	}

	void Scene::UpdateSceneData(const std::shared_ptr<PerspectiveCamera>& camera)
	{
		int shadowMapIndex = 0;
		m_SceneData.NumberOfShadowCastingPointLights = 0;
		m_SceneData.NumberOfShadowCastingSpotLights = 0;
		m_SceneData.NumberOfShadowCastingDirectionalLights = 0;

		m_SceneData.NumberOfPointLights = m_Registry.storage<PointLightComponent>().size();
		if (m_SceneData.NumberOfPointLights)
		{
			auto group = m_Registry.group<PointLightComponent>(entt::get<TransformComponent>);
			for (entt::entity entity : group)
			{
				auto [pointLight, transform] = group.get<PointLightComponent, TransformComponent>(entity);
				pointLight.Position = transform.Position;
			}

			m_SceneData.PointLightComponents = *(m_Registry.storage<PointLightComponent>().raw());
			for (int i = 0; i < m_SceneData.NumberOfPointLights; i++)
			{
				PointLightComponent& pointLight = m_SceneData.PointLightComponents[i];
				if (pointLight.CastShadows)
				{
					if (shadowMapIndex > 7)
					{
						LY_CORE_WARNING("Only 8 point lights can cast shadows");
						pointLight.CastShadows = false;
						pointLight.ShadowMapIndex = -1;
					}

					else
					{
						pointLight.ShadowMapIndex = shadowMapIndex++;
						m_SceneData.NumberOfShadowCastingPointLights++;
					}
				}

				else
				{
					pointLight.ShadowMapIndex = -1;
				}
			}
		}
		else
		{
			m_SceneData.PointLightComponents = nullptr;
		}

		shadowMapIndex = 0;

		m_SceneData.NumberOfSpotLights = m_Registry.storage<SpotLightComponent>().size();
		if (m_SceneData.NumberOfSpotLights)
		{
			auto group = m_Registry.group<SpotLightComponent>(entt::get<TransformComponent>);
			for (entt::entity entity : group)
			{
				auto [spotLight, transform] = group.get<SpotLightComponent, TransformComponent>(entity);
				spotLight.Position = glm::vec4(transform.Position, 0.0f);
			}

			m_SceneData.SpotLightComponents = *(m_Registry.storage<SpotLightComponent>().raw());
			for (int i = 0; i < m_SceneData.NumberOfSpotLights; i++)
			{
				SpotLightComponent& spotLight = m_SceneData.SpotLightComponents[i];
				if (spotLight.CastShadows)
				{
					if (shadowMapIndex > 7)
					{
						LY_CORE_WARNING("Only 8 spot lights can cast shadows");
						spotLight.CastShadows = false;
						spotLight.ShadowMapIndex = -1;
					}
					
					else
					{
						spotLight.ShadowMapIndex = shadowMapIndex++;
						m_SceneData.NumberOfShadowCastingSpotLights++;
					}
				}

				else
				{
					spotLight.ShadowMapIndex = -1;
				}
			}
		}
		else
		{
			m_SceneData.SpotLightComponents = nullptr;
		}

		shadowMapIndex = 0;

		m_SceneData.NumberOfDirectionalLights = m_Registry.storage<DirectionalLightComponent>().size();
		if (m_SceneData.NumberOfDirectionalLights)
		{
			m_SceneData.DirectionalLightComponents = *(m_Registry.storage<DirectionalLightComponent>().raw());
			for (int i = 0; i < m_SceneData.NumberOfDirectionalLights; i++)
			{
				DirectionalLightComponent& directionalLight = m_SceneData.DirectionalLightComponents[i];
				if (directionalLight.CastShadows)
				{
					if (shadowMapIndex > 0)
					{
						LY_CORE_WARNING("Only one directional light can cast shadows");
						directionalLight.CastShadows = false;
						directionalLight.ShadowMapIndex = -1;
					}

					else
					{
						directionalLight.ShadowMapIndex = shadowMapIndex++;
						m_SceneData.NumberOfShadowCastingDirectionalLights++;
					}
				}

				else
				{
					directionalLight.ShadowMapIndex = -1;
				}
			}
		}
		else
		{
			m_SceneData.DirectionalLightComponents = nullptr;
		}

		m_SceneData.Camera = camera;
		//TODO: For now there has to be a sky light component
		LY_CORE_ASSERT(m_Registry.storage<SkyLightComponent>().size() == 1, "Only one sky light component is allowed");

		SkyLightComponent& skyLight = **m_Registry.storage<SkyLightComponent>().raw();
		m_SceneData.EnvironmentMap = skyLight.Cubemap;
		m_SceneData.AmbientStrength = skyLight.Intensity;
	}

}