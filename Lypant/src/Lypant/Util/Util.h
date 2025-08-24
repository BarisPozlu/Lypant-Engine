#pragma once

#include <memory>
#include <string>

namespace lypant
{
	class Image;

	namespace util
	{
		std::shared_ptr<Image> CreateCubemapFromEquirectangularImage(const std::string& path);
	}
}