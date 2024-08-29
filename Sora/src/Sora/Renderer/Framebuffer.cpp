#include "sorapch.h"
#include "Framebuffer.h"

#include "Sora/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Sora {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	SORA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLFramebuffer>(spec);
		}

		SORA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}