#pragma once

#include <memory>
#include <string>

namespace lypant
{
	class Cubemap;
	class Texture2D;

	namespace util
	{
		std::shared_ptr<Cubemap> GetDiffuseIrradianceMap(const std::shared_ptr<Cubemap>& source);
		std::shared_ptr<Cubemap> GetPreFilteredMap(const std::shared_ptr<Cubemap>& source);
		std::shared_ptr<Cubemap> CreateCubemapFromEquirectangularTexture(const std::string& path);
		std::shared_ptr<Texture2D> CreateBRDFIntegrationMap();
	}
}
