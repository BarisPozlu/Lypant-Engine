#pragma once

#include "Sampler.h"

namespace lypant
{
	enum class ImageType
	{
		Unknown, Image2D, Image2DArray, Cubemap, CubemapArray
	};

	enum ImageUsageFlags
	{
		ImageUsageFlagsNone = 0,
		ImageUsageFlagsSample = 1,
		ImageUsageFlagsColorAttachment = 2,
		ImageUsageFlagsDepthAttachment = 4,
		ImageUsageFlagsTransferSrc = 8,
		ImageUsageFlagsTransferDst = 16
	};
	
	// Common image parameters across different types of images.
	struct ImageParams
	{
		SamplerSpecification SamplerSpec;
		bool LinearSpace = true;
		bool GenerateMipMap = false;
		bool FloatingImage = false;
	};

	// Each image type will populate this and pass it to the graphics API implementation.
	struct ImageSpecification
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Depth = 0;
		uint32_t Channels = 0;
		uint32_t Layers = 0;
		uint32_t Samples = 0;
		int UsageFlags = ImageUsageFlagsNone;
		ImageType Type = ImageType::Unknown;
		ImageParams Params;
	};

	// Base class for graphics API specific images.
	// Each image type when created, returns an image handle.
	class Image
	{
	public:
		Image() = default;
		virtual ~Image() = default;
	};

	// Image2D objects are images that can be loaded from a file, or can be given an array of pixels.
	// These images can't be render targets but should be created to sample from. Such as material textures.
	class Image2D
	{
	public:
		// Load image from a file
		static std::shared_ptr<Image> Create(const std::string& path, const ImageParams& params);
		// Create image from already loaded data
		static std::shared_ptr<Image> Create(uint32_t width, uint32_t height, uint32_t channels, const void* data, const ImageParams& params);
	};

	// Cubemaps are only loaded from equirectangular images.
	// Usages are set to be sampled and color attachment. Since they have to be rendered to using equirectangular images.
	class Cubemap
	{
	public:
		// Creates and allocates memory for the cubemap without any data
		static std::shared_ptr<Image> Create(uint32_t width, uint32_t height, uint32_t channels, const ImageParams& params);
	};
}