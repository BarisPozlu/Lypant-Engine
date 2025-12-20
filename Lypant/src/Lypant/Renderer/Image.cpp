#include <lypch.h>
#include "Image.h"
#include "GraphicsContext.h"
#include "Vulkan/VulkanImage.h"

namespace lypant
{
	std::shared_ptr<Image> Image::CreateImage2D(const std::string& path, const ImageParams& params)
	{
		const auto it = s_Cache.find(path);
		if (it != s_Cache.end())
		{
			return std::shared_ptr<Image>(it->second);
		}

		std::shared_ptr<Image> image;

		ImageSpecification spec;
		spec.Type = ImageType::Image2D;
		spec.Params = params;
		spec.Depth = 1;
		spec.Layers = 1;
		spec.Samples = 1;
		spec.UsageFlags = ImageUsageFlagsTransferDst | ImageUsageFlagsSample;
		if (params.GenerateMipMap) spec.UsageFlags |= ImageUsageFlagsTransferSrc;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: image =  std::make_shared<VulkanImage>(path, spec); break;
			default: LY_CORE_ASSERT(false, "Unknown Graphics API");
		}

		s_Cache[path] = std::weak_ptr<Image>(image);
		return image;
	}

	std::shared_ptr<Image> Image::CreateImage2D(uint32_t width, uint32_t height, uint32_t channels, const void* data, const ImageParams& params)
	{
		ImageSpecification spec;
		spec.Type = ImageType::Image2D;
		spec.Width = width;
		spec.Height = height;
		spec.Channels = channels;
		spec.Params = params;
		spec.Depth = 1;
		spec.Layers = 1;
		spec.Samples = 1;
		spec.UsageFlags = ImageUsageFlagsTransferDst | ImageUsageFlagsSample;
		if (params.GenerateMipMap) spec.UsageFlags |= ImageUsageFlagsTransferSrc;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanImage>(data, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Image> Image::CreateImage2DRenderTarget(uint32_t width, uint32_t height, uint32_t channels, const ImageParams& params, bool sampled)
	{
		ImageSpecification spec;
		spec.Type = ImageType::Image2D;
		spec.Width = width;
		spec.Height = height;
		spec.Channels = channels;
		spec.Params = params;
		spec.Depth = 1;
		spec.Layers = 1;
		spec.Samples = 1;
		spec.UsageFlags = ImageUsageFlagsColorAttachment;
		if (sampled) spec.UsageFlags |= ImageUsageFlagsSample;
		if (params.GenerateMipMap) spec.UsageFlags |= ImageUsageFlagsTransferSrc;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanImage>(nullptr, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Image> Image::CreateDepthImage2D(uint32_t width, uint32_t height, const ImageParams& params, bool sampled)
	{
		ImageSpecification spec;
		spec.Type = ImageType::Image2D;
		spec.Width = width;
		spec.Height = height;
		spec.Channels = 1;
		spec.Params = params;
		spec.Depth = 1;
		spec.Layers = 1;
		spec.Samples = 1;
		spec.UsageFlags = ImageUsageFlagsDepthAttachment;
		if (sampled) spec.UsageFlags |= ImageUsageFlagsSample;
		if (params.GenerateMipMap) spec.UsageFlags |= ImageUsageFlagsTransferSrc;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanImage>(nullptr, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}

	std::shared_ptr<Image> Image::CreateCubemap(uint32_t width, uint32_t height, uint32_t channels, const ImageParams& params)
	{
		ImageSpecification spec;
		spec.Type = ImageType::Cubemap;
		spec.Width = width;
		spec.Height = height;
		spec.Channels = channels;
		spec.Params = params;
		spec.Depth = 1;
		spec.Layers = 6;
		spec.Samples = 1;
		spec.UsageFlags = ImageUsageFlagsSample | ImageUsageFlagsColorAttachment;

		switch (GraphicsContext::GetGraphicsAPI())
		{
			case GraphicsAPI::None: LY_CORE_ASSERT(false, "None graphics api is not supported."); break;
			case GraphicsAPI::Vulkan: return std::make_shared<VulkanImage>(nullptr, spec); break;
		}

		LY_CORE_ASSERT(false, "Unknown Graphics API");
		return nullptr;
	}
}
