#pragma once

#include <memory>
#include "Buffer.h"
#include "Material.h"

namespace lypant
{
	class Mesh
	{
	public:
		Mesh(const std::shared_ptr<Buffer>& vertexBuffer, const std::shared_ptr<Buffer>& indexBuffer, const std::shared_ptr<Material>& material)
			: m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer), m_Material(material) {}
		~Mesh() = default;
		inline const std::shared_ptr<Buffer>& GetVertexBuffer() const { return m_VertexBuffer; }
		inline const std::shared_ptr<Buffer>& GetIndexBuffer() const { return m_IndexBuffer; }
		inline const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }
	private:
		std::shared_ptr<Buffer> m_VertexBuffer;
		std::shared_ptr<Buffer> m_IndexBuffer;
		std::shared_ptr<Material> m_Material;
		mutable bool m_Merged = false;
	private: // TODO: Terrbile, fix this later
		friend class VulkanRenderCommandBuffer;
	};
}