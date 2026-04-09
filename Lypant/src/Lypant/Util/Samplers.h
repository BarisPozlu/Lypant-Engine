#pragma once

#include <memory>

namespace lypant
{
	class Sampler;
	class Renderer;

	namespace util
	{
		class Samplers
		{
		public:
			inline static const std::shared_ptr<Sampler>& GetLinearRepeat() { return s_LinearRepeat; }
			inline static const std::shared_ptr<Sampler>& GetLinearClampEdge() { return s_LinearClampEdge; }
		private:
			static void Create();
			static void Destroy();
		private:
			inline static std::shared_ptr<Sampler> s_LinearRepeat;
			inline static std::shared_ptr<Sampler> s_LinearClampEdge;
		private:
			friend class Renderer;
		};
	}
}