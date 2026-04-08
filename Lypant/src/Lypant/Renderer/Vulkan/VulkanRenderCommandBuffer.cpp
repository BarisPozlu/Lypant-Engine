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

	void VulkanRenderCommandBuffer::Draw(const Mesh& mesh, const std::shared_ptr<Shader>& shader, uint32_t instanceCount, bool IsImmediate)
	{
		VkCommandBuffer commandBuffer = IsImmediate ? m_ImmediateCommandBuffer.GetVkCommandBuffer() : GetCurrentFrame().CommandBuffer;

		const auto& vkVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetVertexBuffer());
		const auto& vkIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetIndexBuffer());
		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);

		std::array<VkDeviceAddress, 2> addresses = {vkVertexBuffer->GetDeviceAddress(), vkIndexBuffer->GetDeviceAddress()};

		vkCmdPushConstants(commandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(addresses), addresses.data());

		// TODO: Change the way we get index count
		vkCmdDraw(commandBuffer, vkIndexBuffer->GetSize() / sizeof(uint32_t), instanceCount, 0, 0);
	}

	void VulkanRenderCommandBuffer::Submit(const Mesh& mesh, const glm::mat4& modelMatrix, uint32_t instanceCount)
	{
		// TODO: Right now cannot update model matrix if already merged, again will be fixed with dynamic loading
		if (mesh.m_Merged) return;

		// TODO: Check how material textures were transitioned before, this looks bad

		auto images = mesh.GetMaterial()->GetData().Textures.Get();

		for (auto& image : images)
		{
			reinterpret_cast<std::shared_ptr<VulkanImage>&>(image)->TransitionLayout(GetCurrentFrame().CommandBuffer, { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
		}

		m_DrawData.push_back({ mesh, modelMatrix, instanceCount });
		mesh.m_Merged = true;
	}

	void VulkanRenderCommandBuffer::Execute(const std::shared_ptr<Shader>& shader)
	{
		// NOTE: Execute will only bind the descriptor set and call drawIndirect in the future
		// Merging meshes is going to be different when dynamic loading is implemented
		static bool firstRun = true;

		if (firstRun)
		{
			MergeMeshes();
			firstRun = false;
		}

		const auto& vkShader = reinterpret_cast<const std::shared_ptr<VulkanShader>&>(shader);

		std::array<VkDeviceAddress, 2> addresses = { m_VertexBuffer->GetDeviceAddress(), m_IndexBuffer->GetDeviceAddress() };

		vkCmdPushConstants(GetCurrentFrame().CommandBuffer, vkShader->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(addresses), addresses.data());

		vkCmdBindDescriptorSets(GetCurrentFrame().CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkShader->GetPipelineLayout(), 2, 1, &m_IndirectDescriptorSet->GetVkDescriptorSet(), 0, nullptr);

		vkCmdDrawIndirect(GetCurrentFrame().CommandBuffer, m_IndirectBuffer->GetVkBuffer(), 0, m_DrawData.size(), sizeof(VkDrawIndirectCommand));
	}

	void VulkanRenderCommandBuffer::MergeMeshes()
	{
		// TODO: Destroy the old vertex and index buffers after merging them. What about materials?
		// NOTE: Material index is the same as the mesh index which is going to be given to the Base instance in the indirect buffer

		struct MetaData
		{
			uint32_t VertexOffset = 0;
			uint32_t IndexOffset = 0;
		};

		struct MatrixData
		{
			glm::mat4 ModelMatrix;
			glm::vec3 NormalCol1;
			alignas(16) glm::vec3 NormalCol2;
			alignas(16) glm::vec3 NormalCol3;
		};

		std::vector<VkDrawIndirectCommand> indirectCommands(m_DrawData.size());

		// NOTE: These buffers are static for now will change it later
		// Go down to updating the descriptor set GetSize() won't be the exact size
		std::vector<MetaData> metaData(m_DrawData.size());
		std::vector<MaterialConstants> materialData(m_DrawData.size());
		std::vector<MatrixData> matrixData(m_DrawData.size());
		std::vector<ImageBinding> imageBindings(m_DrawData.size() * 6); // TODO: Each material has 6 textures, get this value properly

		uint32_t totalVertexSize = 0;
		uint32_t totalIndexSize = 0;

		for (int i = 0; i < m_DrawData.size(); i++)
		{
			const Mesh& mesh = m_DrawData[i].MeshData;

			auto images = mesh.GetMaterial()->GetData().Textures.Get();

			for (int j = 0; j < images.size(); j++)
			{
				imageBindings[i * images.size() + j] = { images[j], 0 };
			}

			uint32_t vertexSize = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetVertexBuffer())->GetSize();
			uint32_t indexSize = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetIndexBuffer())->GetSize();

			indirectCommands[i].firstInstance = i;
			indirectCommands[i].firstVertex = 0;
			indirectCommands[i].instanceCount = 1;
			indirectCommands[i].vertexCount = indexSize / sizeof(uint32_t);

			totalVertexSize += vertexSize;
			totalIndexSize += indexSize;
		}

		m_VertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateVertexBuffer(nullptr, totalVertexSize));
		m_IndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateIndexBuffer(nullptr, totalIndexSize));

		BeginImmediateCommands();

		uint32_t vertexBufferOffset = 0;
		uint32_t indexBufferOffset = 0;

		for (int i = 0; i < m_DrawData.size(); i++)
		{
			Mesh& mesh = m_DrawData[i].MeshData;

			materialData[i] = mesh.GetMaterial()->GetData().Constants;

			matrixData[i].ModelMatrix = m_DrawData[i].ModelMatrix;
			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_DrawData[i].ModelMatrix)));
			matrixData[i].NormalCol1 = normalMatrix[0];
			matrixData[i].NormalCol2 = normalMatrix[1];
			matrixData[i].NormalCol3 = normalMatrix[2];

			auto& submeshVertexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetVertexBuffer());
			auto& submeshIndexBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(mesh.GetIndexBuffer());

			VkBufferCopy copy{};

			copy.size = submeshVertexBuffer->GetSize();
			copy.dstOffset = vertexBufferOffset;

			metaData[i].VertexOffset = vertexBufferOffset;

			vertexBufferOffset += submeshVertexBuffer->GetSize();

			vkCmdCopyBuffer(m_ImmediateCommandBuffer.GetVkCommandBuffer(), submeshVertexBuffer->GetVkBuffer(), m_VertexBuffer->GetVkBuffer(), 1, &copy);

			copy.size = submeshIndexBuffer->GetSize();
			copy.dstOffset = indexBufferOffset;

			metaData[i].IndexOffset = indexBufferOffset;

			indexBufferOffset += submeshIndexBuffer->GetSize();

			vkCmdCopyBuffer(m_ImmediateCommandBuffer.GetVkCommandBuffer(), submeshIndexBuffer->GetVkBuffer(), m_IndexBuffer->GetVkBuffer(), 1, &copy);

			mesh.m_VertexBuffer.reset();
			mesh.m_IndexBuffer.reset();
		}

		EndImmediateCommands();

		m_MetaBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateStorageBuffer(sizeof(MetaData) * metaData.size(), metaData.data()));
		m_MatrixBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateStorageBuffer(sizeof(MatrixData) * matrixData.size(), matrixData.data()));
		m_MaterialBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateStorageBuffer(sizeof(MaterialConstants) * materialData.size(), materialData.data()));
		m_IndirectBuffer = reinterpret_cast<const std::shared_ptr<VulkanBuffer>&>(Buffer::CreateIndirectBuffer(sizeof(VkDrawIndirectCommand) * indirectCommands.size(), indirectCommands.data()));

		m_IndirectDescriptorSet->UpdateWithDescriptorArrays(imageBindings, 
			{ { m_MetaBuffer, 1, 0, m_MetaBuffer->GetSize()}, { m_MatrixBuffer, 2, 0, m_MatrixBuffer->GetSize() }, { m_MaterialBuffer, 3, 0, m_MaterialBuffer->GetSize() } });
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

		m_EnvironmentDescriptorSet = std::make_unique<VulkanDescriptorSet>(graphicsContext.GetEnvironmentDescriptorSetLayout());
		m_IndirectDescriptorSet = std::make_unique<VulkanDescriptorSet>(graphicsContext.GetIndirectDescriptorSetLayout());
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