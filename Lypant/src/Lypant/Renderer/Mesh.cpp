#include "lypch.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Model.h"
#include "Lypant/Util/VertexArrays.h"

namespace lypant
{
	Mesh::Mesh(DefaultGeometry geometry, const std::shared_ptr<Material>& material) : m_Material(material)
	{
		if (geometry == DefaultGeometry::Sphere)
		{
			m_VertexArray = util::VertexArrays::GetSphere();
		}

		else if (geometry == DefaultGeometry::Cube)
		{
			m_VertexArray = util::VertexArrays::GetCube();
		}
	}
}
