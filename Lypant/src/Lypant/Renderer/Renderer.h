#pragma once

#include "lypch.h"
#include "RenderCommand.h"
#include "Lypant/Camera/PerspectiveCamera.h"
#include "Material.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "Mesh.h"
#include "Model.h"
#include "Skybox.h"
#include "Lypant/Scene/Scene.h"

namespace lypant
{
	enum class AntiAliasingSetting
	{
		None, MSAA2X, MSAA4X, MSAA8X, MSAA16X
	};

	class Renderer
	{
	public:
		static void BeginScene(const Scene::SceneData& sceneData);
		static void EndScene();
		static void Submit(const Mesh& mesh, const glm::mat4& modelMatrix);
		static void Submit(const std::shared_ptr<Skybox>& skybox);
		// You should set anti aliasing before you call BeginScene().
		static void SetAntiAliasing(AntiAliasingSetting setting);
		static void SetExposure(float exposure);
		static void SetBloom(bool enabled);
	private:
		static void Init(uint32_t windowWidth, uint32_t windowHeight);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void UpdateEnvironmentBuffers(const Scene::SceneData& sceneData);
		static void CreateMSAAFrameBuffer(uint32_t samples);
		static void UpdateMSAAFrameBufferAttachments(uint32_t samples);
		static void CreatePostProcessFrameBuffer();
		static void UpdatePostProcessFrameBufferAttachments();
		static void CreatePostProcessQuadVertexArray();
		static void CreateBloomResources();
		static void DeleteBloomResources();
		static void CreateBloomTexture(const std::shared_ptr<Texture2D>& sceneTexture);
		static void CreateBRDFIntegrationMap();
	private:
		struct RendererData
		{
		public:
			RendererData() {}

			~RendererData()
			{
				delete EnvironmentUniformBuffer;
				delete[] EnvironmentBuffer;

				delete MSAAFrameBuffer;
				delete PostProcessFrameBuffer;

				delete BloomFrameBuffer;
			}
		public:
			std::shared_ptr<Skybox> EnvironmentMap;
			std::shared_ptr<Cubemap> DiffuseIrradianceMap;
			std::shared_ptr<Cubemap> PrefilteredMap;
			std::shared_ptr<Texture2D> BRDFIntegrationMap;

			UniformBuffer* EnvironmentUniformBuffer = nullptr;
			char* EnvironmentBuffer = nullptr;

			uint32_t WindowWidth = 0;
			uint32_t WindowHeight = 0;

			AntiAliasingSetting AntiAliasingSetting = AntiAliasingSetting::None;
			FrameBuffer* MSAAFrameBuffer = nullptr;

			FrameBuffer* PostProcessFrameBuffer = nullptr;
			std::shared_ptr<VertexArray> PostProcessQuadVertexArray;
			std::shared_ptr<Shader> PostProcessShader;
			float Exposure = 1.0f;

			FrameBuffer* BloomFrameBuffer = nullptr;
			std::array<std::shared_ptr<Texture2D>, 6> BloomMipTextures; // more mip levels mean larger bloom radius, can be adjusted here
			std::shared_ptr<Shader> BloomDownsampleShader;
			std::shared_ptr<Shader> BloomUpsampleShader;
			bool IsBloomEnabled = false;
		};

		static RendererData* s_RendererData;
	private:
		friend class Application;
		friend class Scene;
	};
}
