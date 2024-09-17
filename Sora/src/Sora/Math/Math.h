#pragma once

#include <glm/glm.hpp>

namespace Sora { namespace Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& o_Translation, glm::vec3& o_Rotaion, glm::vec3& o_Scale);

} }