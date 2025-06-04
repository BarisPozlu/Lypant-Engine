#pragma once

#include <memory>
#include <string>

namespace lypant
{
	class Cubemap;
	class Texture2D;

	namespace util
	{
		std::shared_ptr<Cubemap> CreateDiffuseIrradianceMap(const std::shared_ptr<Cubemap>& source);
		std::shared_ptr<Cubemap> CreatePreFilteredMap(const std::shared_ptr<Cubemap>& source);
		std::shared_ptr<Cubemap> CreateCubemapFromEquirectangularTexture(const std::string& path);
	}
}
