#include "sorapch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Sora {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}
