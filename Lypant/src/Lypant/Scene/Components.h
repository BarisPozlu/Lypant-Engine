#pragma once

#include <glm/glm.hpp>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Lypant/Renderer/Mesh.h"
#include "Entity.h"

namespace lypant
{
	struct TransformComponent
	{
	public:
		TransformComponent(const glm::vec3& position = glm::vec3(0.0f), const glm::quat& rotation = glm::quat(1, 0, 0, 0), const glm::vec3& scale = glm::vec3(1.0f))
			: Position(position), Rotation(rotation), Scale(scale) { }
		inline glm::mat4 GetModelMatrix() const { return glm::translate(glm::mat4(1.0f), Position) * glm::scale(glm::mat4_cast(Rotation), Scale); }
		inline operator glm::mat4() const { return GetModelMatrix(); }
	public:
		glm::vec3 Position;
		glm::quat Rotation;
		glm::vec3 Scale;
	};

	class BehaviorComponentBase
	{
	public:
		BehaviorComponentBase() = default;
		virtual ~BehaviorComponentBase() = default;
		virtual void BeginScene() {}
		virtual void Tick(float deltaTime) {}
		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}
	private:
		Entity m_Entity;
	private:
		friend class Scene;
	};

	struct MeshComponent
	{
	public:
		MeshComponent(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material)
			: MeshData(vertexArray, material) { }
		MeshComponent(DefaultGeometry geometry, const std::shared_ptr<Material>& material)
			: MeshData(geometry, material) { }
	public:
		Mesh MeshData;
	};

	struct PointLightComponent
	{
	public:
		PointLightComponent(const glm::vec3& color, float linear = 0.0f, float quadratic = 1.0f)
			: Color(color, 0), Linear(linear), Quadratic(quadratic) { }
	public:
		glm::vec4 Color;
	private:
		glm::vec3 Position; // this is supposed to be set by the scene from the transform component before sending it to the renderer
	public:
		float Linear;
		float Quadratic;
	private:
		int ShadowMapIndex = -1; // this is supposed to be set by the scene before sending to the renderer
	public:
		bool CastShadows = true;
	private: // 7 bytes padding needed for gpu.
		bool Padding1;
		bool Padding2;
		bool Padding3;
		float Padding4;
	private:
		friend class Scene;
		friend class Renderer;
		friend class PointLight;
	};

	struct SpotLightComponent
	{
	public:
		SpotLightComponent(const glm::vec3& color, const glm::vec3& direction, float outerCutOff = 17.5, float innerCutOff = 12.5)
			: Color(color, 0), Direction(glm::normalize(direction)),
			OuterCutOff(glm::cos(glm::radians(outerCutOff))), InnerCutOff(glm::cos(glm::radians(innerCutOff))) { }

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::normalize(direction);
		}
	public:
		glm::vec4 Color;
	private:
		glm::vec4 Position; // this is supposed to be set by the scene from the transform component before sending it to the renderer
	public:
		glm::vec3 Direction;
		float OuterCutOff;
		float InnerCutOff;
	private:
		int ShadowMapIndex = -1; // this is supposed to be set by the scene before sending to the renderer
	public:
		bool CastShadows = true;
	private: // 7 bytes padding needed for gpu.
		bool Padding1;
		bool Padding2;
		bool Padding3;
		float Padding4;
	private:
		friend class Scene;
		friend class Renderer;
		friend class SpotLight;
	};

	struct DirectionalLightComponent
	{
	public:
		DirectionalLightComponent(const glm::vec3& color, const glm::vec3& direction) : Color(color, 0), Direction(glm::normalize(direction)) {}

		// normalizes the direction. Advised to use this intead of setting it up directly.
		inline void SetDirection(const glm::vec3& direction)
		{
			Direction = glm::vec4(glm::normalize(direction), 0);
		}
	public:
		glm::vec4 Color;
		glm::vec3 Direction;
	private:
		int ShadowMapIndex = -1; // this is supposed to be set by the scene before sending to the renderer
	public:
		bool CastShadows = true;
	private: // 15 bytes padding needed for gpu.
		bool Padding1;
		bool Padding2;
		bool Padding3;
		float Padding4;
		float Padding5;
		float Padding6;
	private:
		friend class Scene;
		friend class DirectionalLight;
	};

}