#pragma once

namespace lypant
{
	enum class ImageWrappingOption
	{
		Repeat, ClampEdge, ClampBorder
	};

	struct Image2DSpecification
	{
		ImageWrappingOption WrappingOption = ImageWrappingOption::Repeat;
		bool LinearSpace = true;
		bool GenerateMipMap = false;
		bool FloatingImage = false;
	};

	// Image2D objects are images that can be loaded from a file, or can be given an array of pixels
	// These images can't be render targets but should be created to sample from. Such as material textures
	class Image2D
	{
	public:
		Image2D() = default;
		virtual ~Image2D() = default;
		// Load image from a file
		static std::shared_ptr<Image2D> Create(const std::string& path, const Image2DSpecification& spec);
		// Create image from already loaded data
		static std::shared_ptr<Image2D> Create(uint32_t width, uint32_t height, uint32_t channels, void* data, const Image2DSpecification& spec);
	};
}