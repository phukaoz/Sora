#include "sorapch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Sora {

	static GLenum ToGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:		return GL_NONE;
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Mat3:		return GL_FLOAT;
			case ShaderDataType::Mat4:		return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}

		SORA_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		SORA_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &mRendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		SORA_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &mRendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		SORA_PROFILE_FUNCTION();

		glBindVertexArray(mRendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		SORA_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertex_buffer)
	{
		SORA_PROFILE_FUNCTION();
		SORA_CORE_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(mRendererID);
		vertex_buffer->Bind();

		const BufferLayout& layout = vertex_buffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribPointer(
						mVertexBufferIndex,
						element.GetComponentCount(),
						ToGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset
					);
					mVertexBufferIndex++;
					break;
				}

				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(mVertexBufferIndex);
						glVertexAttribPointer(
							mVertexBufferIndex,
							count,
							ToGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.Offset + sizeof(float) * count * i)
						);
						glVertexAttribDivisor(mVertexBufferIndex, 1);
						mVertexBufferIndex++;
					}
					break;
				}

				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(mVertexBufferIndex);
					glVertexAttribIPointer(
						mVertexBufferIndex,
						element.GetComponentCount(),
						ToGLBaseType(element.Type),
						layout.GetStride(),
						(const void*)element.Offset
					);
					mVertexBufferIndex++;
					break;
				}

				default:
					SORA_CORE_ASSERT(false, "Invalid shader data type!");
					break;
			}
			
		}

		mVertexBuffers.push_back(vertex_buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& index_buffer)
	{
		SORA_PROFILE_FUNCTION();

		glBindVertexArray(mRendererID);
		index_buffer->Bind();

		mIndexBuffer = index_buffer;
	}

}
