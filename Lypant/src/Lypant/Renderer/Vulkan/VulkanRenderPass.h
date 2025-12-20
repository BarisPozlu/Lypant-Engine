#pragma once

#include <Lypant/Renderer/RenderPass.h>
#include <vulkan/vulkan.h>
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRenderTarget.h"

// TODO: Draw commands are not recoreded in passes for now. The system will slightly change when that is introduced.

namespace lypant
{
	class VulkanSubpass : public Subpass
	{
	public:
		VulkanSubpass(const std::shared_ptr<RenderTarget>& renderTarget, const RenderTargetOperation& op, const std::shared_ptr<Shader>& shader, const std::vector<ImageBinding>& dataBindings, uint32_t uniformBufferSize, const void* data, bool isDynamic, int subpassFlags);
		virtual ~VulkanSubpass() = default;
		virtual void UploadData(const void* data, uint32_t size, uint32_t offset) override;
		inline virtual void Submit(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount = 1) override { m_DrawData.emplace_back(mesh, modelMatrix, instanceCount); }
		inline virtual void ClearDrawData() override { m_DrawData.clear(); }
		inline virtual const std::vector<DrawData>& GetDrawData() const override { return m_DrawData; }
		inline virtual const std::shared_ptr<Shader>& GetShader() const override { return m_Shader; }
		inline virtual int GetFlags() const override { return m_SubpassFlags; };

		inline const std::shared_ptr<VulkanRenderTarget>& GetRenderTarget() const { return m_RenderTarget; }
		inline const std::shared_ptr<VulkanShader>& GetVkShader() const { return m_Shader; }
		inline const std::vector<ImageBinding>& GetDataBindings() const { return m_DataBindings; }
		inline const std::shared_ptr<VulkanGraphicsPipeline>& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
		inline const std::shared_ptr<VulkanBuffer>& GetUniformBuffer() const { return m_UniformBuffer; }
		inline const std::shared_ptr<VulkanDescriptorSet>& GetDescriptorSet() const { return m_DescriptorSet; }
		inline const RenderTargetOperation& GetRenderTargetOperation() const { return m_RenderTargetOperation; }
		
	private:
		std::shared_ptr<VulkanRenderTarget> m_RenderTarget;
		std::shared_ptr<VulkanShader> m_Shader;
		std::vector<ImageBinding> m_DataBindings;
		std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
		std::shared_ptr<VulkanBuffer> m_UniformBuffer;
		std::shared_ptr<VulkanDescriptorSet> m_DescriptorSet;
		std::vector<DrawData> m_DrawData;
		int m_SubpassFlags;
		RenderTargetOperation m_RenderTargetOperation;
	};

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass();
		virtual ~VulkanRenderPass();
	private:
		std::vector<VulkanSubpass> m_Subpasses;
		int m_CurrentPass = -1;
	};
}