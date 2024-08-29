#include "Yukipch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Yuki {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPosition[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData sData;

	void Renderer2D::Init()
	{
		YUKI_PROFILE_FUNCTION();

		sData.QuadVertexArray = VertexArray::Create();
		sData.QuadVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(QuadVertex));
		sData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
		});
		sData.QuadVertexArray->AddVertexBuffer(sData.QuadVertexBuffer);

		sData.QuadVertexBufferBase = new QuadVertex[sData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[Renderer2DData::MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MaxIndices);
		sData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		sData.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		sData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t sampler[Renderer2DData::MaxTextureSlots];
		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
			sampler[i] = i;

		sData.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		sData.TextureShader->Bind();
		sData.TextureShader->SetIntArray("u_Textures", sampler, Renderer2DData::MaxTextureSlots);

		// Set white texture at index 0.
		sData.TextureSlots[0] = sData.WhiteTexture;

		sData.QuadVertexPosition[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		sData.QuadVertexPosition[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		sData.QuadVertexPosition[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		sData.QuadVertexPosition[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		YUKI_PROFILE_FUNCTION();

		delete[] sData.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		YUKI_PROFILE_FUNCTION();

		glm::mat4 view_proj = camera.GetProjection() * glm::inverse(transform);

		sData.TextureShader->Bind();
		sData.TextureShader->SetMat4("u_ViewProjection", view_proj);

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		YUKI_PROFILE_FUNCTION();

		sData.TextureShader->Bind();
		sData.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		YUKI_PROFILE_FUNCTION();

		glm::mat4 view_proj = camera.GetViewProjection();

		sData.TextureShader->Bind();
		sData.TextureShader->SetMat4("u_ViewProjection", view_proj);

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		YUKI_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)sData.QuadVertexBufferPtr - (uint8_t*)sData.QuadVertexBufferBase);
		sData.QuadVertexBuffer->SetData(sData.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < sData.TextureSlotIndex; i++)
			sData.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(sData.QuadVertexArray, sData.QuadIndexCount);
		sData.Stats.DrawCallCount++;
	}

	void Renderer2D::StartBatch()
	{
		sData.QuadIndexCount = 0;
		sData.QuadVertexBufferPtr = sData.QuadVertexBufferBase;

		sData.TextureSlotIndex = 1;
	}
	
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	//////////////////
	//				//
	//	   DRAW		//
	//				//
	//////////////////

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation /*= 0.0f*/)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation /*= 0.0f*/)
	{
		YUKI_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0.0f) transform *= glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f});
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation /*= 0.0f*/, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, rotation, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation /*= 0.0f*/, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		YUKI_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0.0f) transform *= glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float rotation /*= 0.0f*/, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, rotation, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float rotation /*= 0.0f*/, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		YUKI_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (sData.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < sData.TextureSlotIndex; i++)
		{
			if (*sData.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)sData.TextureSlotIndex;
			sData.TextureSlots[sData.TextureSlotIndex] = texture;
			sData.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		if (rotation != 0.0f) transform *= glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f });
		transform *= glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			sData.QuadVertexBufferPtr->Position = transform * sData.QuadVertexPosition[i];
			sData.QuadVertexBufferPtr->Color = color;
			sData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			sData.QuadVertexBufferPtr->TexIndex = textureIndex;
			sData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			sData.QuadVertexBufferPtr++;
		}

		sData.QuadIndexCount += 6;

		sData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		YUKI_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float textureIndex = 0.0f;
		const float tilingFactor = 1.0f;

		if (sData.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			sData.QuadVertexBufferPtr->Position = transform * sData.QuadVertexPosition[i];
			sData.QuadVertexBufferPtr->Color = color;
			sData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			sData.QuadVertexBufferPtr->TexIndex = textureIndex;
			sData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			sData.QuadVertexBufferPtr++;
		}

		sData.QuadIndexCount += 6;

		sData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor /*= 1.0f*/, const glm::vec4& tintColor /*= glm::vec4(1.0f)*/)
	{
		YUKI_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (sData.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < sData.TextureSlotIndex; i++)
		{
			if (*sData.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)sData.TextureSlotIndex;
			sData.TextureSlots[sData.TextureSlotIndex] = texture;
			sData.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			sData.QuadVertexBufferPtr->Position = transform * sData.QuadVertexPosition[i];
			sData.QuadVertexBufferPtr->Color = color;
			sData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			sData.QuadVertexBufferPtr->TexIndex = textureIndex;
			sData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			sData.QuadVertexBufferPtr++;
		}

		sData.QuadIndexCount += 6;

		sData.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&sData.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return sData.Stats;
	}

}