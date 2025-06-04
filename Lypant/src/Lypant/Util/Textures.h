#pragma once

#include <memory>

namespace lypant
{
	class Texture2D;

	namespace util
	{
		// the purpose of the class is to provide an easy way to get some common textures
		// all textures are created and deleted by the application for now
		class Textures
		{
		public:
			inline static const std::shared_ptr<Texture2D>& GetWhite3Channel1x1() { return s_White3Channel1x1; }
			inline static const std::shared_ptr<Texture2D>& GetWhite1Channel1x1() { return s_White1Channel1x1; }
			inline static const std::shared_ptr<Texture2D>& GetBRDFIntegrationMap() { return s_BRDFIntegrationMap; }
		private:
			// creates all the textures
			static void Create();
			// destroys all the textures
			static void Destroy();
		private:
			inline static std::shared_ptr<Texture2D> s_White3Channel1x1;
			inline static std::shared_ptr<Texture2D> s_White1Channel1x1;
			inline static std::shared_ptr<Texture2D> s_BRDFIntegrationMap;
		private:
			friend class Application;
		};
	}
}
