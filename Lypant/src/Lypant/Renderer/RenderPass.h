#pragma once

#include <vector>
#include <memory>
#include "Image.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderTarget.h"

namespace lypant
{
	struct DataBinding
	{
		std::shared_ptr<Image> Image;
		uint32_t Binding;
	};

	class Subpass
	{
	public:
		virtual ~Subpass() = default;
		static std::unique_ptr<Subpass> Create(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings);
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		static std::unique_ptr<RenderPass> Create(const std::vector<Subpass>& subpasses);
	};
}