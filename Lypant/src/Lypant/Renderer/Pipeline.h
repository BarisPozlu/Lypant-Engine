#pragma once

#include "Shader.h"

namespace lypant
{
	struct GraphicsPipelineSpecification
	{

	};

	class GraphicsPipeline
	{
	public:
		static std::shared_ptr<GraphicsPipeline> Create(const GraphicsPipelineSpecification& spec, const std::shared_ptr<Shader>& shader, const std::shared_ptr<class RenderTarget>& renderTarget);
		virtual ~GraphicsPipeline() = default;
	};
}