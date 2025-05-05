#include "lypch.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Model.h"

namespace lypant
{
	std::weak_ptr<VertexArray> Mesh::s_CubeVertexArray;
	std::weak_ptr<VertexArray> Mesh::s_SphereVertexArray;

	Mesh::Mesh(DefaultGeometry geometry, const std::shared_ptr<Material>& material) : m_Material(material)
	{
		switch (geometry)
		{
		case DefaultGeometry::Cube:
			if (s_CubeVertexArray.expired())
			{
				std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();

				float vertexData[]
				{
					-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
					 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
					 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
					-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,

					-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
					 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
					-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
					-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

					-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
					-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
					-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
					-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
					-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

					 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
					 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
					 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

					-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
					 0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
					 0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
					-0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
					-0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

					-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
					 0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
					-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
					-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f
				};

				std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

				BufferLayout layout
				{
					{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float2, "a_TexCoord"}
				};

				vertexBuffer->SetLayout(layout);
				vertexArray->AddVertexBuffer(vertexBuffer);

				// draw arrays are not supported, we have to use indices.
				unsigned int indexData[36];
				for (int i = 0; i < 36; i++)
				{
					indexData[i] = i;
				}

				std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 36);

				vertexArray->SetIndexBuffer(indexBuffer);

				s_CubeVertexArray = vertexArray;
				m_VertexArray.swap(vertexArray);
			}

			else
			{
				std::shared_ptr<VertexArray> vertexArray(s_CubeVertexArray);
				m_VertexArray.swap(vertexArray);
			}

			break;

			case DefaultGeometry::Sphere:
				if (s_SphereVertexArray.expired())
				{
					Model model("models/sphere/sphere.glb", false);
					m_VertexArray = model.GetMeshes()[0].GetVertexArray();
					s_SphereVertexArray = m_VertexArray;
				}
				
				else
				{
					std::shared_ptr<VertexArray> vertexArray(s_SphereVertexArray);
					m_VertexArray.swap(vertexArray);
				}
		}
	}
}
