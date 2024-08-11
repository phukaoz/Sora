#include "Yukipch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Yuki {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		YUKI_PROFILE_FUNCTION();

		m_DataFormat = GL_RGBA;
		m_InternalFormat = GL_RGBA8;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		YUKI_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data;
		{
			YUKI_PROFILE_SCOPE("stbi_load() - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		YUKI_CORE_ASSERT(data, "Failed to load image: {0}", path);
		m_Width = width;
		m_Height = height;

		switch (channels)
		{
		case 1:
			m_DataFormat = GL_RED;
			m_InternalFormat = GL_R8;
			break;
		case 2:
			m_DataFormat = GL_RG;
			m_InternalFormat = GL_RG8;
			break;
		case 3:
			m_DataFormat = GL_RGB;
			m_InternalFormat = GL_RGB8;
			break;
		case 4:
			m_DataFormat = GL_RGBA;
			m_InternalFormat = GL_RGBA8;
			break;
		default:
			m_DataFormat = GL_RGBA;
			m_InternalFormat = GL_RGBA8;
			break;
		}
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		YUKI_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		YUKI_PROFILE_FUNCTION();

		//TODO: assert size = width * height * bpp
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		YUKI_PROFILE_FUNCTION();

		glBindTextureUnit(0, m_RendererID);
	}

}