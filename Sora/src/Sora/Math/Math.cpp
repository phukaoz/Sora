#include "sorapch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Sora::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& o_Tanslation, glm::vec3& o_Rotation, glm::vec3& o_Scale)
	{
		using namespace glm;

		mat4 localMatrix(transform);

		if (epsilonEqual(localMatrix[3][3], 0.0f, epsilon<float>()))
			return false;

		if (
			epsilonNotEqual(localMatrix[0][3], 0.0f, epsilon<float>()) ||
			epsilonNotEqual(localMatrix[1][3], 0.0f, epsilon<float>()) ||
			epsilonNotEqual(localMatrix[2][3], 0.0f, epsilon<float>()))
		{
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
			localMatrix[3][3] = 1.0f;
		}

		o_Tanslation = vec3(localMatrix[3]);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 row[3];

		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				row[i][j] = localMatrix[i][j];

		o_Scale.x = length(row[0]);
		row[0] = detail::scale(row[0], 1.0f);
		o_Scale.y = length(row[1]);
		row[1] = detail::scale(row[1], 1.0f);
		o_Scale.z = length(row[2]);
		row[2] = detail::scale(row[2], 1.0f);
		
		o_Rotation.y = asin(-row[0][2]);
		if (cos(o_Rotation.y) != 0)
		{
			o_Rotation.x = atan2(row[1][2], row[2][2]);
			o_Rotation.z = atan2(row[0][1], row[0][0]);
		}
		else
		{
			o_Rotation.x = atan2(-row[2][0], row[1][1]);
			o_Rotation.z = 0;
		}

		return true;
	}

}