#pragma once

#include <memory>
#include "Image.h"

namespace lypant
{
	enum class AttachmentLoadOperation
	{
		Load, Clear, DontCare
	};

	enum class AttachmentStoreOperation
	{
		Store, DontCare
	};

	// TODO: For now render targets only support 1 color buffer but it is easy to extend
	struct RenderTargetOperation
	{
		AttachmentLoadOperation ColorBufferLoadOp = AttachmentLoadOperation::DontCare;
		AttachmentStoreOperation ColorBufferStoreOp = AttachmentStoreOperation::DontCare;
		AttachmentLoadOperation DepthBufferLoadOp = AttachmentLoadOperation::DontCare;
		AttachmentStoreOperation DepthBufferStoreOp = AttachmentStoreOperation::DontCare;
	};

	class RenderTarget
	{
	public:
		static std::shared_ptr<RenderTarget> Create();
		static std::shared_ptr<RenderTarget> GetDefault();
		virtual ~RenderTarget() = default;
		virtual void AttachColorBuffer(const std::shared_ptr<Image>& image, int mipLevel = 0) = 0;
		virtual void AttachDepthStencilBuffer(const std::shared_ptr<Image>& image) = 0;
	};
}