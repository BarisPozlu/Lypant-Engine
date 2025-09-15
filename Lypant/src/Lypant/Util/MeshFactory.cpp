#include <lypch.h>
#include "MeshFactory.h"

namespace lypant
{
	namespace util
	{
		static std::shared_ptr<Mesh> CreateCubemapCube()
		{
			//float vertexData[]
			//{
			//	-1.0f,  1.0f,  1.0f, 1.0f,
			//	 1.0f,  1.0f,  1.0f, 1.0f,
			//	 1.0f, -1.0f,  1.0f, 1.0f,
			//	 1.0f, -1.0f,  1.0f, 1.0f,
			//	-1.0f, -1.0f,  1.0f, 1.0f,
			//	-1.0f,  1.0f,  1.0f, 1.0f,

			//	-1.0f,  1.0f, -1.0f, 1.0f,
			//	 1.0f,  1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f, -1.0f, 1.0f,
			//	-1.0f, -1.0f, -1.0f, 1.0f,
			//	-1.0f,  1.0f, -1.0f, 1.0f,

			//	 1.0f,  1.0f,  1.0f, 1.0f,
			//	 1.0f,  1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f,  1.0f, 1.0f,
			//	 1.0f,  1.0f,  1.0f, 1.0f,

			//	-1.0f,  1.0f,  1.0f, 1.0f,
			//	-1.0f,  1.0f, -1.0f, 1.0f,
			//	-1.0f, -1.0f, -1.0f, 1.0f,
			//	-1.0f, -1.0f, -1.0f, 1.0f,
			//	-1.0f, -1.0f,  1.0f, 1.0f,
			//	-1.0f,  1.0f,  1.0f, 1.0f,

			//	-1.0f,  1.0f, -1.0f, 1.0f,
			//	 1.0f,  1.0f, -1.0f, 1.0f,
			//	 1.0f,  1.0f,  1.0f, 1.0f,
			//	 1.0f,  1.0f,  1.0f, 1.0f,
			//	-1.0f,  1.0f,  1.0f, 1.0f,
			//	-1.0f,  1.0f, -1.0f, 1.0f,

			//	-1.0f, -1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f, -1.0f, 1.0f,
			//	 1.0f, -1.0f,  1.0f, 1.0f,
			//	 1.0f, -1.0f,  1.0f, 1.0f,
			//	-1.0f, -1.0f,  1.0f, 1.0f,
			//	-1.0f, -1.0f, -1.0f, 1.0f,
			//};

			float vertexData[]
			{
				-1.0f,  1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, 1.0f,
				 1.0f, -1.0f, -1.0f, 1.0f,
				 1.0f, -1.0f, -1.0f, 1.0f,
				 1.0f,  1.0f, -1.0f, 1.0f,
				-1.0f,  1.0f, -1.0f, 1.0f,

				-1.0f, -1.0f,  1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, 1.0f,
				-1.0f,  1.0f, -1.0f, 1.0f,
				-1.0f,  1.0f, -1.0f, 1.0f,
				-1.0f,  1.0f,  1.0f, 1.0f,
				-1.0f, -1.0f,  1.0f, 1.0f,

				 1.0f, -1.0f, -1.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f, -1.0f, 1.0f,
				 1.0f, -1.0f, -1.0f, 1.0f,

				-1.0f, -1.0f,  1.0f, 1.0f,
				-1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 1.0f,
				-1.0f, -1.0f,  1.0f, 1.0f,

				-1.0f,  1.0f, -1.0f, 1.0f,
				 1.0f,  1.0f, -1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				 1.0f,  1.0f,  1.0f, 1.0f,
				-1.0f,  1.0f,  1.0f, 1.0f,
				-1.0f,  1.0f, -1.0f, 1.0f,

				-1.0f, -1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f,  1.0f, 1.0f,
				 1.0f, -1.0f, -1.0f, 1.0f,
				 1.0f, -1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f,  1.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 1.0f
			};

			std::shared_ptr<Buffer> vertexBuffer = Buffer::CreateVertexBuffer(vertexData, sizeof(vertexData));

			constexpr int indexCount = 36;

			unsigned int indexData[indexCount];
			for (int i = 0; i < indexCount; i++)
			{
				indexData[i] = i;
			}

			std::shared_ptr<Buffer> indexBuffer = Buffer::CreateIndexBuffer(indexData, indexCount);

			return std::make_shared<Mesh>(vertexBuffer, indexBuffer, nullptr);
		}

		static std::shared_ptr<Mesh> CreateQuad()
		{
			//float vertexData[]
			//{
			//	-1.0f,	1.0f, 0.0f, 1.0f,
			//	 1.0f,	1.0f, 1.0f, 1.0f,
			//	 1.0f, -1.0f, 1.0f, 0.0f,
			//	-1.0f, -1.0f, 0.0f, 0.0f
			//};

			float vertexData[]
			{
				-1.0f, -1.0f, 0.0f, 0.0f,
				 1.0f, -1.0f, 1.0f, 0.0f,
				 1.0f,  1.0f, 1.0f, 1.0f,
				-1.0f,  1.0f, 0.0f, 1.0f
			};

			uint32_t indexData[]
			{
				0, 1, 2, 2, 3, 0
			};

			std::shared_ptr<Buffer> vertexBuffer = Buffer::CreateVertexBuffer(vertexData, sizeof(vertexData));

			std::shared_ptr<Buffer> indexBuffer = Buffer::CreateIndexBuffer(indexData, 6);

			return std::make_shared<Mesh>(vertexBuffer, indexBuffer, nullptr);
		}

		void MeshFactory::Create()
		{
			s_CubemapCube = CreateCubemapCube();
			s_Quad = CreateQuad();
		}

		void MeshFactory::Destroy()
		{
			s_Quad.reset();
			s_CubemapCube.reset();
		}
	}
}