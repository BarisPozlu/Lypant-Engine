#include "lypch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "BehaviorComponent.h"
#include "Lypant/Renderer/Renderer.h"

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

	void Scene::Tick(float deltaTime, const std::shared_ptr<PerspectiveCamera>& camera, const std::shared_ptr<Skybox>& skybox)
	{
		UpdateSceneData(camera, skybox);

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
		// Only one directional light will cast shadows, I will have to update this in the future after I test shadows in a broader scene
		for (int i = 0; i < m_SceneData.NumberOfDirectionalLights; i++)
		{
			const DirectionalLight light(m_SceneData.DirectionalLightComponents[i]);

			Renderer::BeginShadowPass(light, *m_SceneData.Camera);

			for (entt::entity entity : group)
			{
				auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
				Renderer::SubmitForShadowPass(mesh.MeshData, transform);
			}
		}

		Renderer::EndShadowPass();
		
		// 3D Render Pass

		Renderer::BeginScene(m_SceneData);

		for (entt::entity entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			Renderer::Submit(mesh.MeshData, transform);
		}

		Renderer::EndScene();
	}

	void Scene::UpdateSceneData(const std::shared_ptr<PerspectiveCamera>& camera, const std::shared_ptr<Skybox>& skybox)
	{
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
		}
		else
		{
			m_SceneData.PointLightComponents = nullptr;
		}

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
		}
		else
		{
			m_SceneData.SpotLightComponents = nullptr;
		}

		m_SceneData.NumberOfDirectionalLights = m_Registry.storage<DirectionalLightComponent>().size();
		if (m_SceneData.NumberOfDirectionalLights)
		{
			m_SceneData.DirectionalLightComponents = *(m_Registry.storage<DirectionalLightComponent>().raw());
		}
		else
		{
			m_SceneData.DirectionalLightComponents = nullptr;
		}

		m_SceneData.Camera = camera;
		m_SceneData.Skybox = skybox;
	}

}