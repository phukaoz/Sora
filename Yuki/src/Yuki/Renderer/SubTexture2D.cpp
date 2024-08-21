#include "Yukipch.h"
#include "SubTexture2D.h"

namespace Yuki {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{  
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	Ref<SubTexture2D> SubTexture2D::Create(const Ref<Texture2D>& texture, const glm::vec2& coords,const glm::vec2& tileSize, const glm::vec2& spriteSize/*={ 1, 1 }*/)
	{
		glm::vec2 min = { (coords.x * tileSize.x) / texture->GetWidth(), (coords.y * tileSize.y) / texture->GetHeight() };
		glm::vec2 max = { ((coords.x + spriteSize.x) * tileSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * tileSize.y) / texture->GetHeight() };
		return CreateRef<SubTexture2D>(texture, min, max);
	}

}