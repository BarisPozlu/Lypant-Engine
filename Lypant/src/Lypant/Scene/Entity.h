#pragma once

#include "lypch.h"
#include "Scene.h"

// make the asserts core when not included by the application

namespace lypant
{
	class Entity
	{
	public:
		Entity() : m_EntityHandle(entt::null), m_Scene(nullptr) {}
		Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

		template<typename T, typename...Args>
		T& AddComponent(Args&&...args)
		{
			LY_ASSERT(!HasComponent<T>(), "Entity already has the specified component");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			LY_ASSERT(HasComponent<T>(), "Entity does not have the specified component");
			return m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			LY_ASSERT(HasComponent<T>(), "Entity does not have the specified component");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		inline operator bool() const { return m_EntityHandle != entt::null; }
	private:
		entt::entity m_EntityHandle;
		Scene* m_Scene;
	};
}
