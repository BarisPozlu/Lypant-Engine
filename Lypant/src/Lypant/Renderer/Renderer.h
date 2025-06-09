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
		static void BeginShadowPass(const Scene::SceneData& sceneData, LightType lightType);
		static void EndShadowPass();
		static void SubmitForShadowPass(const Mesh& mesh, const glm::mat4& modelMatrix, LightType lightType, int count);
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
		static void CreateBloomResources();
		static void DeleteBloomResources();
		static void CreateBloomTexture(const std::shared_ptr<Texture2D>& sceneTexture);
		static std::vector<glm::vec4> GetWorldPositionOfFrustumCorners(const glm::mat4& viewProjectionMatrix);
		static void CalculateDirectionalLightSpaceMatrices(const DirectionalLightComponent& light, const PerspectiveCamera& camera);
		static void CalculateDirectionalLightSpaceMatrix(const DirectionalLightComponent& light, const PerspectiveCamera& camera, float nearPlane, float farPlane, int cascade);
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

				delete ShadowMapFrameBuffer;
			}
		public:
			// IBL data
			std::shared_ptr<Skybox> EnvironmentMap;
			std::shared_ptr<Cubemap> DiffuseIrradianceMap;
			std::shared_ptr<Cubemap> PrefilteredMap;
			std::shared_ptr<Texture2D> BRDFIntegrationMap;

			// Light/camera data
			UniformBuffer* EnvironmentUniformBuffer = nullptr;
			char* EnvironmentBuffer = nullptr;

			uint32_t WindowWidth = 0;
			uint32_t WindowHeight = 0;

			// Anti-aliasing data
			AntiAliasingSetting AntiAliasingSetting = AntiAliasingSetting::None; 
			FrameBuffer* MSAAFrameBuffer = nullptr;

			// Post-process data
			FrameBuffer* PostProcessFrameBuffer = nullptr; 
			std::shared_ptr<VertexArray> PostProcessQuadVertexArray;
			std::shared_ptr<Shader> PostProcessShader;
			float Exposure = 1.0f;

			// Bloom data
			FrameBuffer* BloomFrameBuffer = nullptr; 
			std::array<std::shared_ptr<Texture2D>, 6> BloomMipTextures; // more mip levels mean larger bloom radius, can be adjusted here
			std::shared_ptr<Shader> BloomDownsampleShader;
			std::shared_ptr<Shader> BloomUpsampleShader;
			bool IsBloomEnabled = false;

			// Shadow map data
			FrameBuffer* ShadowMapFrameBuffer = nullptr;
			// Only one directional light can cast shadows
			std::shared_ptr<Shader> CascadedShadowMapShader;
			std::shared_ptr<Texture2DArray> DirectionalLightShadowMaps;
			std::array<glm::mat4, 5> DirectionalLightSpaceMatrices;
			std::array<float, 5> CascadePlaneDistances;
			// At most 8 spot lights can cast shadows
			std::shared_ptr<Shader> DirectionalShadowMapShader;
			std::shared_ptr<Texture2DArray> SpotLightShadowMaps;
			std::array<glm::mat4, 8> SpotLightSpaceMatrices;
			// At most 8 point lights can cast shadows
			std::shared_ptr<Shader> OmnidirectionalShadowMapShader;
			std::shared_ptr<CubemapArray> PointLightShadowMaps;
		};

		static RendererData* s_RendererData;
	private:
		friend class Application;
		friend class Scene;
	};
}
