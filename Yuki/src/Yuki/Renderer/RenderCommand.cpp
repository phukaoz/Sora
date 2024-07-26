#include "Yukipch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Yuki {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}
