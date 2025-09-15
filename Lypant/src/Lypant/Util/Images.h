#pragma once

#include <memory>

namespace lypant
{
	class Image;
	class Renderer;

	namespace util
	{
		class Images
		{
		public:
			inline static const std::shared_ptr<Image>& GetWhite4Channel1x1() { return s_White4Channel1x1; }
			inline static const std::shared_ptr<Image>& GetWhite1Channel1x1() { return s_White1Channel1x1; }
			inline static const std::shared_ptr<Image>& GetBRDFIntegrationMap() { return s_BRDFIntegrationMap; }
		private:
			static void Create();
			static void Destroy();
		private:
			inline static std::shared_ptr<Image> s_White4Channel1x1;
			inline static std::shared_ptr<Image> s_White1Channel1x1;
			inline static std::shared_ptr<Image> s_BRDFIntegrationMap;
		private:
			friend class Renderer;
		};
	}
}