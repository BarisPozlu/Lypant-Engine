#pragma once

#include <memory>

namespace lypant
{
	enum class GraphicsAPI
	{
		None, Vulkan
	};

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		static std::unique_ptr<GraphicsContext> Create(void* window);
		inline static void SetGraphicsAPI(GraphicsAPI API) { s_GraphicsAPI = API; }
		inline static GraphicsAPI GetGraphicsAPI() { return s_GraphicsAPI; }
	private:
		static GraphicsAPI s_GraphicsAPI;
	};
}