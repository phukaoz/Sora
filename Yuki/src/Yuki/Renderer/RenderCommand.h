#pragma once

#include "RendererAPI.h"

namespace Yuki {

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
		inline static void DrawIndexed(const Ref<VertexArray>& vertex_array, std::optional<uint32_t> count = std::nullopt)
		{
			s_RendererAPI->DrawIndexed(vertex_array, count);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}
