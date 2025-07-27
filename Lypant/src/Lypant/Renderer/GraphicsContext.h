#pragma once

#include <memory>

namespace lypant
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		static std::unique_ptr<GraphicsContext> Create(void* window);
	};
}