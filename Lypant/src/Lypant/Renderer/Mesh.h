#pragma once

#include <memory>
#include "Buffer.h"
#include "Material.h"

namespace lypant
{
	class Mesh
	{
	public:
		Mesh(const std::shared_ptr<VertexBuffer> vertexBuffer, const std::shared_ptr<IndexBuffer> indexBuffer, std::shared_ptr<Material> material)
			: m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_Material(material) {}
		~Mesh() = default;
		inline const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
		inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
		inline const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }
	private:
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Material> m_Material;
	};
}