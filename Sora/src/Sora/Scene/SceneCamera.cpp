#include "sorapch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp> 

namespace Sora {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		mProjectionType = ProjectionType::Orthographic;
		mOrthographicSize = size;
		mOrthographicNear = nearClip;
		mOrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		mProjectionType = ProjectionType::Perspective;
		mPerspectiveFOV = verticalFov;
		mPerspectiveNear = nearClip;
		mPerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		mAspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		switch (mProjectionType)
		{
		case Sora::SceneCamera::ProjectionType::Orthographic:
		{
			float orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
			float orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
			float orthoBottom = -mOrthographicSize * 0.5f;
			float orthoTop = mOrthographicSize * 0.5f;

			mProjection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
			break;
		}
		case Sora::SceneCamera::ProjectionType::Perspective:
		{
			mProjection = glm::perspective(mPerspectiveFOV, mAspectRatio, mPerspectiveNear, mPerspectiveFar);
			break;
		}
		}
		
	}

}