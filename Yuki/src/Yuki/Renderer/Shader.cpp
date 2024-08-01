#include "Yukipch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Yuki {

	Yuki::Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	YUKI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	return new OpenGLShader(filepath);
		}
	}

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	YUKI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		YUKI_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
