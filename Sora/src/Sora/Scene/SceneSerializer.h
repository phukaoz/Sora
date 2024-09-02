#pragma once

#include "Scene.h"

#include <filesystem>

namespace Sora {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);
	private:
		Ref<Scene> mScene;
	};

}