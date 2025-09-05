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
		static std::unique_ptr<Subpass> Create(const std::shared_ptr<RenderTarget>& renderTarget, bool shouldClear, const std::shared_ptr<Shader>& shader, const std::vector<DataBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic = false);
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) = 0;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		static std::unique_ptr<RenderPass> Create(const std::vector<Subpass>& subpasses);
	};
}