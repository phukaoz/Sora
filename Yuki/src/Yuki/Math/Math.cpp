#include "Yukipch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Yuki::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& out_translation, glm::vec3& out_rotation, glm::vec3& out_scale)
	{
		using namespace glm;

		mat4 local_matrix(transform);

		if (epsilonEqual(local_matrix[3][3], 0.0f, epsilon<float>()))
			return false;

		if (
			epsilonNotEqual(local_matrix[0][3], 0.0f, epsilon<float>()) ||
			epsilonNotEqual(local_matrix[1][3], 0.0f, epsilon<float>()) ||
			epsilonNotEqual(local_matrix[2][3], 0.0f, epsilon<float>()))
		{
			local_matrix[0][3] = local_matrix[1][3] = local_matrix[2][3] = 0.0f;
			local_matrix[3][3] = 1.0f;
		}

		out_translation = vec3(local_matrix[3]);
		local_matrix[3] = vec4(0, 0, 0, local_matrix[3].w);

		vec3 row[3];

		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				row[i][j] = local_matrix[i][j];

		out_scale.x = length(row[0]);
		row[0] = detail::scale(row[0], 1.0f);
		out_scale.y = length(row[1]);
		row[1] = detail::scale(row[1], 1.0f);
		out_scale.z = length(row[2]);
		row[2] = detail::scale(row[2], 1.0f);
		
		out_rotation.y = asin(-row[0][2]);
		if (cos(out_rotation.y) != 0)
		{
			out_rotation.x = atan2(row[1][2], row[2][2]);
			out_rotation.z = atan2(row[0][1], row[0][0]);
		}
		else
		{
			out_rotation.x = atan2(-row[2][0], row[1][1]);
			out_rotation.z = 0;
		}

		return true;
	}

}