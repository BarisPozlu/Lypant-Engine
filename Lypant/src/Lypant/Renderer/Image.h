#pragma once

#include "Sampler.h"

namespace lypant
{
	enum class ImageType
	{
		Image1D, Image2D, Image3D
	};

	enum ImageUsageFlags
	{
		ImageUsageFlagsNone = 0,
		ImageUsageFlagsSample = 1,
		ImageUsageFlagsColorAttachment = 2,
		ImageUsageFlagsDepthAttachment = 4
	};
	
	// Base class for graphics API specific images.
	// Each image type when created, returns an image handle.
	class Image
	{
	public:
		Image() = default;
		virtual ~Image() = default;
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
		ImageUsageFlags UsageFlags = ImageUsageFlagsNone;
		ImageParams Params;
	};

	// Image2D objects are images that can be loaded from a file, or can be given an array of pixels.
	// These images can't be render targets but should be created to sample from. Such as material textures.
	class Image2D
	{
	public:
		Image2D() = default;
		virtual ~Image2D() = default;
		// Load image from a file
		static std::shared_ptr<Image> Create(const std::string& path, const ImageParams& spec);
		// Create image from already loaded data
		static std::shared_ptr<Image> Create(uint32_t width, uint32_t height, uint32_t channels, const void* data, const ImageParams& spec);
	};
}