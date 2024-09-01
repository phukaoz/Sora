#include "sorapch.h"
#include "UniformBuffer.h"

#include "Sora/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Sora {

	Sora::Ref<Sora::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    SORA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		SORA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}