#include "sorapch.h"
#include "SceneSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "Entity.h"
#include "Component.h"

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& right)
		{
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.push_back(right.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& right)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			right.x = node[0].as<float>();
			right.y = node[1].as<float>();
			right.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& right)
		{
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.push_back(right.z);
			node.push_back(right.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& right)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			right.x = node[0].as<float>();
			right.y = node[1].as<float>();
			right.z = node[2].as<float>();
			right.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Sora {

	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vector)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vector.x << vector.y << vector.z << YAML::EndSeq;
		return out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vector)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vector.x << vector.y << vector.z << vector.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "0";
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}
		
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			{
				auto& transform_component = entity.GetComponent<TransformComponent>();
				auto& translation = transform_component.Translation;
				auto& rotation = transform_component.Rotation;
				auto& scale = transform_component.Scale;

				out << YAML::Key << "Translation" << YAML::Value << translation;
				out << YAML::Key << "Rotation" << YAML::Value << rotation;
				out << YAML::Key << "Scale" << YAML::Value << scale;
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;
			{
				auto& sprite_component = entity.GetComponent<SpriteComponent>();
				auto& color = sprite_component.Color;

				out << YAML::Key << "Color" << YAML::Value << color;
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			{
				auto& camera_component = entity.GetComponent<CameraComponent>();
				auto& camera = camera_component.Camera;
				auto& primary = camera_component.Primary;
				auto& fixed_aspect_ratio = camera_component.FixedAspectRatio;

				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap;
				{
					out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();

					out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
					out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
					out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

					out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
					out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
					out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
				}
				out << YAML::EndMap;

				out << YAML::Key << "Primary" << YAML::Value << camera_component.Primary;
				out << YAML::Key << "FixedAspectRatio" << YAML::Value << camera_component.FixedAspectRatio;
			}
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.view<entt::entity>().each([&](auto entity_id)
			{
				Entity entity = { entity_id, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		SORA_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream str_stream;
		str_stream << stream.rdbuf();

		YAML::Node data = YAML::Load(str_stream.str());
		if (!data["Scene"])
			return false;

		std::string scene_name = data["Scene"].as<std::string>();
		SORA_CORE_TRACE("Deserializing scene '{0}'", scene_name);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
					name = tag_component["Tag"].as<std::string>();

				SORA_CORE_TRACE("Deserialized entity witg ID = {0}, name = {1}", uuid, name);

				Entity deserialized_entity = m_Scene->CreateEntity(name);

				auto transform_component = entity["TransformComponent"];
				if (transform_component)
				{
					auto& componet = deserialized_entity.GetComponent<TransformComponent>();
					componet.Translation = transform_component["Translation"].as<glm::vec3>();
					componet.Rotation = transform_component["Rotation"].as<glm::vec3>();
					componet.Scale = transform_component["Scale"].as<glm::vec3>();
				}

				auto camera_component = entity["CameraComponent"];
				if (camera_component)
				{
					auto& component = deserialized_entity.AddComponent<CameraComponent>();

					auto camera_props = camera_component["Camera"];
					component.Camera.SetProjectionType((SceneCamera::ProjectionType)camera_props["ProjectionType"].as<int>());
				
					component.Camera.SetOrthographicSize(camera_props["OrthographicSize"].as<float>());
					component.Camera.SetOrthographicNearClip(camera_props["OrthographicNear"].as<float>());
					component.Camera.SetOrthographicFarClip(camera_props["OrthographicFar"].as<float>());

					component.Camera.SetPerspectiveVerticalFOV(camera_props["PerspectiveFOV"].as<float>());
					component.Camera.SetPerspectiveNearClip(camera_props["PerspectiveNear"].as<float>());
					component.Camera.SetPerspectiveFarClip(camera_props["PerspectiveFar"].as<float>());

					component.Primary = camera_component["Primary"].as<bool>();
					component.FixedAspectRatio = camera_component["FixedAspectRatio"].as<bool>();
				}

				auto sprite_component = entity["SpriteComponent"];
				if (sprite_component)
				{
					auto& component = deserialized_entity.AddComponent<SpriteComponent>();
					component.Color = sprite_component["Color"].as<glm::vec4>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		SORA_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}