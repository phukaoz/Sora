#pragma once

#include "RendererAPI.h"

namespace Sora {

	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		/**
		 * @brief Draws an object.
		 *
		 * This function renders an object using the specified vertex array
		 *
		 * @param vertex_array A reference to the VertexArray containing vertex data and index buffer
		 * @param count The number of indices to use for drawing. If not provided, it uses the entire index buffer
		 */
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, std::optional<uint32_t> indexCount = std::nullopt)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

		inline static void SetDepthWrite(bool enabled)
		{
			s_RendererAPI->SetDepthWrite(enabled);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}
