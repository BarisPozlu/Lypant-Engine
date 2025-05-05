#pragma once

#include "Shader.h"
#include "FrameBufferAttachmentBase.h"

// this is not supposed to be included in the application in the future when the engine matures.
// when that happens make the asserts core again.

namespace lypant
{
	struct BufferSubBlock
	{
		BufferSubBlock(ShaderDataType type, const std::string& name) 
			: Name(name), Type(type), Size(GetSizeFromShaderDataType(type)), Offset(0)
		{

		}

		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
	};

	class BufferLayout
	{
	public:
		BufferLayout() : m_Stride(0)
		{

		}

		BufferLayout(const std::initializer_list<BufferSubBlock>& blocks) : m_Blocks(blocks)
		{
			m_Stride = 0;

			for (BufferSubBlock& block : m_Blocks)
			{
				block.Offset = m_Stride;
				m_Stride += block.Size;
			}
		}

		inline const std::vector<BufferSubBlock>& GetBlocks() const { return m_Blocks; }
		inline uint32_t GetStride() const { return m_Stride; }
	private:
		std::vector<BufferSubBlock> m_Blocks;
		uint32_t m_Stride;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer();
		void Bind() const;
		void Unbind() const;

		inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
		inline const BufferLayout& GetLayout() const { return m_Layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* data, uint32_t count);
		~IndexBuffer();
		void Bind() const;
		void Unbind() const;

		inline uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer(uint32_t size, const void* data);
		~UniformBuffer();
		
		void BindBase(uint32_t index) const;
		void BindRange(uint32_t index, uint32_t offset, uint32_t size) const;
		void BufferSubData(uint32_t offset, uint32_t size, const void* data) const;

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t m_RendererID;
	};

	class RenderBuffer : public DepthStencilAttachment
	{
	public:
		RenderBuffer(int width, int height);
		~RenderBuffer();
	};

	class RenderBufferMultiSample : public DepthStencilAttachment
	{
	public:
		RenderBufferMultiSample(int width, int height, int samples);
		~RenderBufferMultiSample();
	};

	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

		static void BindDefaultFrameBuffer();
		void Bind() const;
		void BindDraw() const;
		void BindRead() const;

		inline uint32_t GetColorBufferWidth() const
		{
			LY_ASSERT(m_ColorAttachment, "There is not a colorbuffer attached to the framebuffer.");
			return m_ColorAttachment->GetWidth();
		};

		inline uint32_t GetColorBufferHeight() const
		{
			LY_ASSERT(m_ColorAttachment, "There is not a colorbuffer attached to the framebuffer.");
			return m_ColorAttachment->GetHeight();
		}

		inline const std::shared_ptr<ColorAttachment>& GetColorBuffer() { return m_ColorAttachment; }
		inline const std::shared_ptr<DepthStencilAttachment>& GetDepthStencilBuffer() { return m_DepthStencilAttachment; }

		// cubemapface is only relevant if you are attaching a cubemap as the ColorAttachment.
		void AttachColorBuffer(const std::shared_ptr<ColorAttachment>& attachment, int level = 0, int cubemapFace = -1);
		void AttachDepthStencilBuffer(const std::shared_ptr<DepthStencilAttachment>& attachment);

		void BlitToFrameBuffer(FrameBuffer* otherBuffer, int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1) const;
		void BlitToDefault(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1) const;
	private:
		uint32_t m_RendererID;
		std::shared_ptr<ColorAttachment> m_ColorAttachment;
		std::shared_ptr<DepthStencilAttachment> m_DepthStencilAttachment;
	};
}
