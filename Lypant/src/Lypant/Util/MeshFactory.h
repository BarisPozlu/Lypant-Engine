#pragma once

#include <Lypant/Renderer/Mesh.h>

namespace lypant
{
	namespace util
	{
		class MeshFactory
		{
		public:
			inline static const std::shared_ptr<Mesh>& GetCubemapCube() { return s_CubemapCube; }
			inline static const std::shared_ptr<Mesh>& GetQuad() { return s_Quad; }
		private:
			static void Create();
			static void Destroy();
		private:
			inline static std::shared_ptr<Mesh> s_CubemapCube;
			inline static std::shared_ptr<Mesh> s_Quad;
		private:
			friend class Application;
		};
	}
}