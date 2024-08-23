#include "Yukipch.h"
#include "Framebuffer.h"

#include "Yuki/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Yuki {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	YUKI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLFramebuffer>(spec);
		}

		YUKI_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}