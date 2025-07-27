#pragma once

#include <memory>

namespace lypant
{
	enum class GraphicsAPI
	{
		None, Vulkan
	};

	class Renderer
	{
	public:
		static std::unique_ptr<Renderer> Create();
		inline static void SetGraphicsAPI(GraphicsAPI API) { s_GraphicsAPI = API; }
		inline static GraphicsAPI GetGraphicsAPI() { return s_GraphicsAPI; }
		virtual ~Renderer() = default;
	protected:
		Renderer();
		virtual void BeginRendering_Impl() = 0;
		virtual void EndRendering_Impl() = 0;
		//TODO: change this from vulkan image to image
		virtual void ClearImage_Impl() = 0;
	public:
		inline static void BeginRendering() { s_Instance->BeginRendering_Impl(); };
		inline static void EndRendering() { s_Instance->EndRendering_Impl(); }
		inline static void ClearImage() { s_Instance->ClearImage_Impl(); }
	private:
		static Renderer* s_Instance;
		static GraphicsAPI s_GraphicsAPI;
	};
}
