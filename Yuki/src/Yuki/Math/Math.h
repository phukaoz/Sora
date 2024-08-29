#pragma once

#include <glm/glm.hpp>

namespace Yuki { namespace Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& out_translation, glm::vec3& out_rotaion, glm::vec3& out_scale);

} }