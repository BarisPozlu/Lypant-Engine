#pragma once

#include <memory>
#include <string>

namespace lypant
{
	class Image;

	namespace util
	{
		std::shared_ptr<Image> CreateCubemapFromEquirectangularImage(const std::string& path);
		std::shared_ptr<Image> CreateDiffuseIrradianceMap(const std::shared_ptr<Image>& source);
		std::shared_ptr<Image> CreatePreFilteredMap(const std::shared_ptr<Image>& source);
	}
}