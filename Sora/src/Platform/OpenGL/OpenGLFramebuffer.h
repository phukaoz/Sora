#pragma once

#include "Sora/Renderer/Framebuffer.h"

namespace Sora {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override 
		{ 
			SORA_CORE_ASSERT(index < mColorAttachments.size(), "Index {0} is out of bounds. There are {1} color attachment(s).", index, mColorAttachments.size()); 
			return mColorAttachments[index]; 
		}

		virtual const FramebufferSpecification& GetSpecification() const override { return mSpecification; }

		virtual void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachment_index, int x, int y) override;
	private:
		uint32_t mRendererID = 0;
		FramebufferSpecification mSpecification;

		std::vector<FramebufferTextureSpecification> mColorAttachmentSpecifications;
		FramebufferTextureSpecification mDepthAttachmentSpecifications = FramebufferTextureFormat::None;

		std::vector<uint32_t> mColorAttachments;
		uint32_t mDepthAttachment;
	};

}