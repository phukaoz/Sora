#include "sorapch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

namespace Sora {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: mWidth(width), mHeight(height)
	{
		SORA_PROFILE_FUNCTION();

		mDataFormat = GL_RGBA;
		mInternalFormat = GL_RGBA8;

		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, GL_RGB8, mWidth, mHeight);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: mTexturePath(path)
	{
		SORA_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data;
		{
			SORA_PROFILE_SCOPE("stbi_load() - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		SORA_CORE_ASSERT(data, "Failed to load image: {0}", path);
		mWidth = width;
		mHeight = height;

		switch (channels)
		{
		case 1:
			mDataFormat = GL_RED;
			mInternalFormat = GL_R8;
			break;
		case 2:
			mDataFormat = GL_RG;
			mInternalFormat = GL_RG8;
			break;
		case 3:
			mDataFormat = GL_RGB;
			mInternalFormat = GL_RGB8;
			break;
		case 4:
			mDataFormat = GL_RGBA;
			mInternalFormat = GL_RGBA8;
			break;
		default:
			mDataFormat = GL_RGBA;
			mInternalFormat = GL_RGBA8;
			break;
		}
		
		glCreateTextures(GL_TEXTURE_2D, 1, &mRendererID);
		glTextureStorage2D(mRendererID, 1, mInternalFormat, mWidth, mHeight);

		glTextureParameteri(mRendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(mRendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(mRendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		SORA_PROFILE_FUNCTION();

		glDeleteTextures(1, &mRendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		SORA_PROFILE_FUNCTION();

		//TODO: assert size = width * height * bpp
		glTextureSubImage2D(mRendererID, 0, 0, 0, mWidth, mHeight, mDataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		SORA_PROFILE_FUNCTION();

		glBindTextureUnit(slot, mRendererID);
	}

}