#include <lypch.h>
#include "VulkanRenderCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanMaterial.h"
#include "Lypant/Renderer/EnvironmentBufferLayout.h"

namespace lypant
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		CreateCommandResources();
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		DestroyCommandResources();
	}

	bool VulkanRenderCommandBuffer::BeginCommands()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		vkWaitForFences(graphicsContext.GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence, false, UINT64_MAX);

		graphicsContext.GetDeletionQueue().Flush();

		bool swapChainResult = graphicsContext.GetSwapChain().OnFrameBegin(GetCurrentFrame().ImageReceivedSemaphore);

		if (!swapChainResult)
		{
			graphicsContext.RecreateSwapChain();
			return false;
		}

		vkResetCommandBuffer(GetCurrentFrame().CommandBuffer, 0);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(GetCurrentFrame().CommandBuffer, &commandBufferBeginInfo);

		return true;
	}

	void VulkanRenderCommandBuffer::EndCommands()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& swapChain = graphicsContext.GetSwapChain();

		swapChain.OnFrameEnd(GetCurrentFrame().CommandBuffer);

		vkEndCommandBuffer(GetCurrentFrame().CommandBuffer);

		vkResetFences(graphicsContext.GetDevice(), 1, &GetCurrentFrame().FrameFinishedFence);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &GetCurrentFrame().ImageReceivedSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &GetCurrentFrame().CommandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[swapChain.GetCurrentImageIndex()];

		vkQueueSubmit(graphicsContext.GetGraphicsQueue(), 1, &submitInfo, GetCurrentFrame().FrameFinishedFence);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[swapChain.GetCurrentImageIndex()];
		presentInfo.swapchainCount = 1;
		VkSwapchainKHR vkSwapChain = swapChain.GetSwapChain();
		presentInfo.pSwapchains = &vkSwapChain;
		uint32_t imageIndex = swapChain.GetCurrentImageIndex();
		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(graphicsContext.GetGraphicsQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) graphicsContext.RecreateSwapChain();

		graphicsContext.m_CurrentFrameIndex = (graphicsContext.m_CurrentFrameIndex + 1) % VulkanGraphicsContext::s_MaxFramesInFlight;
	}

	void VulkanRenderCommandBuffer::BeginSubpass(const Subpass& subpass, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& graphicsContext = VulkanGraphicsContext::Get();

		const VulkanSubpass& vulkanSubpass = reinterpret_cast<const VulkanSubpass&>(subpass);

		VkPipeline pipeline = vulkanSubpass.GetGraphicsPipeline()->GetVkPipeline();
		const auto& shader = vulkanSubpass.GetVkShader();
		const auto& descriptorSet = vulkanSubpass.GetDescriptorSet();
		const auto& renderTarget = vulkanSubpass.GetRenderTarget();
		const auto& uniformBuffer = vulkanSubpass.GetUniformBuffer();

		if (m_EnvironmentBufferSize)
		{
			uint32_t dynamicOffset = m_EnvironmentBufferSize * graphicsContext.GetCurrentFrameIndex();
			m_DynamicOffsets.fill(dynamicOffset);
			const auto& environmentSet = m_EnvironmentDescriptorSet->GetVkDescriptorSet();
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &environmentSet, m_DynamicOffsets.size(), m_DynamicOffsets.data());
		}

		for (const auto& dataBinding : vulkanSubpass.GetDataBindings())
		{
			const auto& vkImage = reinterpret_cast<const std::shared_ptr<VulkanImage>&>(dataBinding.Image);
			vkImage->TransitionLayout(commandBuffer, { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
		}

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		if (descriptorSet)
		{
			const auto& vkDescriptorSet = descriptorSet->GetVkDescriptorSet();
			if (uniformBuffer && uniformBuffer->IsDynamic())
			{
				uint32_t dynamicOffset = vulkanSubpass.GetUniformBuffer()->GetSize() * graphicsContext.GetCurrentFrameIndex();
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 1, 1, &vkDescriptorSet, 1, &dynamicOffset);
			}
			
			else
			{
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 1, 1, &vkDescriptorSet, 0, nullptr);
			}
		}

		const VkRenderingInfo& renderingInfo = renderTarget->PrepareForRendering(commandBuffer, vulkanSubpass.GetRenderTargetOperation());

		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		VkViewport viewport{};
		viewport.x = renderingInfo.renderArea.offset.x;
		viewport.y = renderingInfo.renderArea.extent.height - renderingInfo.renderArea.offset.y;
		//viewport.y = renderingInfo.renderArea.offset.y;
		viewport.width = renderingInfo.renderArea.extent.width;
		viewport.height = -1 * static_cast<float>(renderingInfo.renderArea.extent.height);
		//viewport.height = static_cast<float>(renderingInfo.renderArea.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &renderingInfo.renderArea);
	}

	void VulkanRenderCommandBuffer::EndSubpass(const Subpass& subpass, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		vkCmdEndRendering(commandBuffer);
	}

	void VulkanRenderCommandBuffer::ExecuteSubpass(Subpass& subpass, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		BeginSubpass(subpass, IsImmediate);

		//const VulkanSubpass& vkSubpass = reinterpret_cast<const VulkanSubpass&>(subpass);

		if (subpass.GetFlags() & SubpassFlagDrawWithMaterial)
		{
			for (const DrawData& draw : subpass.GetDrawData())
			{
				// upload the model matrix and material data here but how do we match offsets?
				// for now to make stuff work simply assume that model offset is 0 and normal offset is 64, they will always exist for now for material passes
				// in the future I might implement a system where I give the data type and its name that is stored within the uniform buffer
				// and we can simply get the offset of the variables using maybe a map

				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(draw.ModelMatrix)));
				const auto& vkMaterial = reinterpret_cast<const std::shared_ptr<VulkanMaterial>&>(draw.MeshData.GetMaterial());

				subpass.UploadData(&draw.ModelMatrix, sizeof(draw.ModelMatrix), 0);
				
				// MAT3 cannot be uploadded in one go since each vec3 has an alignment of 16
				subpass.UploadData(&normalMatrix[0], sizeof(glm::vec3), 64);
				subpass.UploadData(&normalMatrix[1], sizeof(glm::vec3), 64 + sizeof(glm::vec4));
				subpass.UploadData(&normalMatrix[2], sizeof(glm::vec3), 64 + 2 * sizeof(glm::vec4));
				
				subpass.UploadData(&vkMaterial->GetData().Constants, sizeof(vkMaterial->GetData().Constants), 112); // padding required here, terrible but will change later
				
				DrawMeshWithMaterial(draw.MeshData, subpass.GetShader(), draw.ModelMatrix, draw.InstanceCount, IsImmediate);
			}
		}

		else
		{
			for (const DrawData& draw : subpass.GetDrawData())
			{
				DrawMesh(draw.MeshData, subpass.GetShader(), draw.ModelMatrix, draw.InstanceCount, IsImmediate);
			}
		}

		EndSubpass(subpass, IsImmediate);
	}

	void VulkanRenderCommandBuffer::DrawMesh(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& vkVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetVertexBuffer());
		const auto& vkIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetIndexBuffer());
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		VkDeviceAddress vertexBufferAddress = vkVertexBuffer->GetDeviceAddress();

		vkCmdPushConstants(commandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &vertexBufferAddress);
		vkCmdBindIndexBuffer(commandBuffer, vkIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// TODO: Change the way we get index count
		vkCmdDrawIndexed(commandBuffer, vkIndexBuffer->GetSize() / sizeof(uint32_t), instanceCount, 0, 0, 0);
	}

	void VulkanRenderCommandBuffer::DrawMeshWithMaterial(const Mesh& mesh, const std::shared_ptr<Shader>& shader, const glm::mat4& modelMatrix, uint32_t instanceCount, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& vkVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetVertexBuffer());
		const auto& vkIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetIndexBuffer());
		const auto& vkMaterial = reinterpret_cast<const std::shared_ptr<VulkanMaterial>&>(mesh.GetMaterial());
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);
		VkDeviceAddress vertexBufferAddress = vkVertexBuffer->GetDeviceAddress();

		vkCmdPushConstants(commandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &vertexBufferAddress);
		vkCmdBindIndexBuffer(commandBuffer, vkIndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

		const auto& materialSet = vkMaterial->GetDescriptorSet()->GetVkDescriptorSet();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader->GetPipelineLayout(), 2, 1, &materialSet, 0, nullptr);

		// TODO: Change the way we get index count
		vkCmdDrawIndexed(commandBuffer, vkIndexBuffer->GetSize() / sizeof(uint32_t), instanceCount, 0, 0, 0);
	}

	void VulkanRenderCommandBuffer::PushData(const void* data, uint32_t size, const std::shared_ptr<Shader>& shader, int shaderStageFlags, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);

		vkCmdPushConstants(commandBuffer, vkShader->GetPipelineLayout(), GetVkShaderStageFlags(shaderStageFlags), sizeof(VkDeviceAddress), size, data);
	}

	void VulkanRenderCommandBuffer::BindEnvironmentBuffer(const std::shared_ptr<Buffer>& buffer)
	{
		std::vector<BufferBinding> bufferBindings(EnvironmentBufferLayout::s_BindingCount);

		for (int i = 0; i < bufferBindings.size(); i++)
		{
			BufferBinding& bufferBinding = bufferBindings[i];
			bufferBinding.Binding = i;
			bufferBinding.Buffer = buffer;
			bufferBinding.Offset = EnvironmentBufferLayout::GetOffsetOfBinding(i);
			bufferBinding.Range = EnvironmentBufferLayout::GetRangeOfBinding(i);
		}

		m_EnvironmentDescriptorSet->Update({ }, bufferBindings);
		m_EnvironmentBufferSize = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(buffer)->GetSize();
	}

	void VulkanRenderCommandBuffer::CreateCommandResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = graphicsContext.GetGraphicsQueueFamilyIndex();

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < VulkanGraphicsContext::s_MaxFramesInFlight; i++)
		{
			vkCreateCommandPool(graphicsContext.GetDevice(), &commandPoolInfo, nullptr, &m_FrameData[i].CommandPool);
			commandBufferInfo.commandPool = m_FrameData[i].CommandPool;
			vkAllocateCommandBuffers(graphicsContext.GetDevice(), &commandBufferInfo, &m_FrameData[i].CommandBuffer);

			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &m_FrameData[i].ImageReceivedSemaphore);
			vkCreateFence(graphicsContext.GetDevice(), &fenceInfo, nullptr, &m_FrameData[i].FrameFinishedFence);
		}

		m_RenderFinishedSemaphores.resize(graphicsContext.GetSwapChain().GetImageCount());

		for (auto& semaphore : m_RenderFinishedSemaphores)
		{
			vkCreateSemaphore(graphicsContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
		}

		m_EnvironmentDescriptorSet = std::make_unique<VulkanDescriptorSet>(graphicsContext.GetGlobalDescriptorSetLayout());
	}

	void VulkanRenderCommandBuffer::DestroyCommandResources()
	{
		auto& graphicsContext = VulkanGraphicsContext::Get();
		auto& frameData = m_FrameData;
		auto& renderFinishedSemaphores = m_RenderFinishedSemaphores;

		graphicsContext.GetDeletionQueue().PushFunction([frameData, renderFinishedSemaphores]()
			{
				auto& graphicsContext = VulkanGraphicsContext::Get();

				for (int i = 0; i < VulkanGraphicsContext::s_MaxFramesInFlight; i++)
				{
					vkDestroyCommandPool(graphicsContext.GetDevice(), frameData[i].CommandPool, nullptr);
					vkDestroyFence(graphicsContext.GetDevice(), frameData[i].FrameFinishedFence, nullptr);
					vkDestroySemaphore(graphicsContext.GetDevice(), frameData[i].ImageReceivedSemaphore, nullptr);
				}

				for (auto& semaphore : renderFinishedSemaphores)
				{
					vkDestroySemaphore(graphicsContext.GetDevice(), semaphore, nullptr);
				}
			});
	}
}