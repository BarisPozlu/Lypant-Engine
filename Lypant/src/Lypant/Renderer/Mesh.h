#pragma once

#include "lypch.h"

namespace lypant
{
	class VertexArray;
	class Material;

	enum class DefaultGeometry
	{
		Cube, Sphere
	};

	class Mesh
	{
	public:
		Mesh(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Material>& material)
			: m_VertexArray(vertexArray), m_Material(material) { }
		Mesh(DefaultGeometry geometry, const std::shared_ptr<Material>& material);

		inline const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		inline const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }

		inline void SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray) { m_VertexArray = vertexArray; }
		inline void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
	private:
		static std::weak_ptr<VertexArray> s_CubeVertexArray;
		static std::weak_ptr<VertexArray> s_SphereVertexArray;
	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Material> m_Material;
	};
}
