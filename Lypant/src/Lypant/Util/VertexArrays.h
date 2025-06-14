#pragma once

#include <memory>

namespace lypant
{
	class VertexArray;

	namespace util
	{
		// the purpose of the class is to provide and easy way to get some common vertex arrays
		// all vertex arrays are created and deleted by the application for now
		class VertexArrays
		{
		public:
			// location 0: position, location 1: texCoords, location 2: normal, location 3: tangent
			inline static const std::shared_ptr<VertexArray>& GetSphere() { return s_Sphere; }
			// location 0: position, location 1: texCoords, location 2: normal, location 3: tangent
			inline static const std::shared_ptr<VertexArray>& GetCube() { return s_Cube; }
			// location 0: position, location 1: texCoords
			inline static const std::shared_ptr<VertexArray>& GetQuad() { return s_Quad; }
			//location 0: position, note: winding order is opposite here because it is supoosed to be rendered from the inside
			inline static const std::shared_ptr<VertexArray>& GetCubemapCube() { return s_CubemapCube; }
		private:
			// creates all the vertex arrays
			static void Create();
			// destroys all the vertex arrays
			static void Destroy();
		private:
			inline static std::shared_ptr<VertexArray> s_Sphere;
			inline static std::shared_ptr<VertexArray> s_Cube;
			inline static std::shared_ptr<VertexArray> s_Quad;
			inline static std::shared_ptr<VertexArray> s_CubemapCube;
		private:
			friend class Application;
		};
	}
}
