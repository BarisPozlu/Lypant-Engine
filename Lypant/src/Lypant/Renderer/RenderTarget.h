#pragma once

#include <memory>
#include "Image.h"

namespace lypant
{
	class RenderTarget
	{
	public:
		static std::shared_ptr<RenderTarget> Create();
		static std::shared_ptr<RenderTarget> GetDefault();
		virtual ~RenderTarget() = default;
		virtual void AttachColorBuffer(const std::shared_ptr<Image>& image) = 0;
		virtual void AttachDepthStencilBuffer(const std::shared_ptr<Image>& image) = 0;
	};
}