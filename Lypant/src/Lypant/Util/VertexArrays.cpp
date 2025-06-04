#include "lypch.h"
#include "VertexArrays.h"
#include "Lypant/Renderer/Model.h"
#include "Lypant/Renderer/VertexArray.h"

namespace lypant
{
	namespace util
	{
		static std::shared_ptr<VertexArray> CreateSphere()
		{
			Model model("models/sphere/sphere.glb", false);
			return model.GetMeshes()[0].GetVertexArray();
		}

		static std::shared_ptr<VertexArray> CreateCube()
		{
			Model model("models/cube/cube.glb", false);
			return model.GetMeshes()[0].GetVertexArray();
		}

		static std::shared_ptr<VertexArray> CreateQuad()
		{
			std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();

			float vertexData[]
			{
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
				 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
				-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
			};

			std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

			BufferLayout layout
			{
				{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}
			};

			vertexBuffer->SetLayout(layout);

			vertexArray->AddVertexBuffer(vertexBuffer);

			unsigned int indexData[]
			{
				0, 1, 2,
				2, 3, 0
			};

			std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 6);
			vertexArray->SetIndexBuffer(indexBuffer);

			return vertexArray;
		}

		static std::shared_ptr<VertexArray> CreateCubemapCube()
		{
			std::shared_ptr<VertexArray> vertexArray = std::make_shared<VertexArray>();

			// TODO: Update the vertex data not to contain duplicate vertices
			float vertexData[]
			{
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(vertexData, sizeof(vertexData));

			BufferLayout layout
			{
				{ShaderDataType::Float3, "a_Position"}
			};

			vertexBuffer->SetLayout(layout);

			vertexArray->AddVertexBuffer(vertexBuffer);

			unsigned int indexData[36];
			for (int i = 0; i < 36; i++)
			{
				indexData[i] = i;
			}

			std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(indexData, 36);
			vertexArray->SetIndexBuffer(indexBuffer);

			return vertexArray;
		}

		void VertexArrays::Create()
		{
			s_Sphere = CreateSphere();
			s_Cube = CreateCube();
			s_Quad = CreateQuad();
			s_CubemapCube = CreateCubemapCube();
		}

		void VertexArrays::Destroy()
		{
			s_Sphere.reset();
			s_Cube.reset();
			s_Quad.reset();
			s_CubemapCube.reset();
		}
	}
}