#include "lypch.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Buffer.h"

namespace lypant
{
	std::weak_ptr<VertexArray> Mesh::s_CubeVertexArray;

	Mesh::Mesh(DefaultGeometry geometry, const std::shared_ptr<Material>& material)
	{
		m_Material = material;

		switch (geometry)
		{
		case DefaultGeometry::Cube:
			if (s_CubeVertexArray.expired())
			{
				std::shared_ptr<lypant::VertexArray> vertexArray = std::make_shared<lypant::VertexArray>();

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

				std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<lypant::VertexBuffer>(vertexData, sizeof(vertexData));

				lypant::BufferLayout layout
				{
					{lypant::ShaderDataType::Float3, "a_Position"}, {lypant::ShaderDataType::Float3, "a_Normal"}, {lypant::ShaderDataType::Float2, "a_TexCoord"}
				};

				vertexBuffer->SetLayout(layout);
				vertexArray->AddVertexBuffer(vertexBuffer);

				// draw arrays are not supported, we have to use indices.
				unsigned int indexData[36];
				for (int i = 0; i < 36; i++)
				{
					indexData[i] = i;
				}

				std::shared_ptr<lypant::IndexBuffer> indexBuffer = std::make_shared<lypant::IndexBuffer>(indexData, 36);

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
		}
	}
}
