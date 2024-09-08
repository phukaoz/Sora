#include "sorapch.h"
#include "SceneSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "Entity.h"
#include "Component.h"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& right)
		{
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& right)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			right.x = node[0].as<float>();
			right.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& right)
		{
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.push_back(right.z);
			node.SetStyle(EmitterStyle::Flow);
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
			node.SetStyle(EmitterStyle::Flow);
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

	static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vector)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vector.x << vector.y << YAML::EndSeq;
		return out;
	}

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
		: mScene(scene)
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity;
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				out << YAML::Key << "Tag" << YAML::Value << tag;
			
				out << YAML::EndMap;
			}
		}
		
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			{
				auto& transformComponent = entity.GetComponent<TransformComponent>();
				auto& translation = transformComponent.Translation;
				auto& rotation = transformComponent.Rotation;
				auto& scale = transformComponent.Scale;

				out << YAML::Key << "Translation"	<< YAML::Value << translation;
				out << YAML::Key << "Rotation"		<< YAML::Value << rotation;
				out << YAML::Key << "Scale"			<< YAML::Value << scale;
			
				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;
			{
				auto& spriteComponent = entity.GetComponent<SpriteComponent>();
				auto& color = spriteComponent.Color;
				auto& texture = spriteComponent.Texture;

				out << YAML::Key << "Color"	<< YAML::Value << color;
				if (texture.get())
					out << YAML::Key << "Texture" << YAML::Value << texture->GetTexturePath().string();

				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;
				auto& primary = cameraComponent.Primary;
				auto& fixedAspectRatio = cameraComponent.FixedAspectRatio;

				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap;
				{
					out << YAML::Key << "ProjectionType"	<< YAML::Value << (int)camera.GetProjectionType();

					out << YAML::Key << "OrthographicSize"	<< YAML::Value << camera.GetOrthographicSize();
					out << YAML::Key << "OrthographicNear"	<< YAML::Value << camera.GetOrthographicNearClip();
					out << YAML::Key << "OrthographicFar"	<< YAML::Value << camera.GetOrthographicFarClip();

					out << YAML::Key << "PerspectiveFOV"	<< YAML::Value << camera.GetPerspectiveVerticalFOV();
					out << YAML::Key << "PerspectiveNear"	<< YAML::Value << camera.GetPerspectiveNearClip();
					out << YAML::Key << "PerspectiveFar"	<< YAML::Value << camera.GetPerspectiveFarClip();

					out << YAML::EndMap;
				}

				out << YAML::Key << "Primary"			<< YAML::Value << cameraComponent.Primary;
				out << YAML::Key << "FixedAspectRatio"	<< YAML::Value << cameraComponent.FixedAspectRatio;

				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;
			{
				auto& spriteComponent	= entity.GetComponent<Rigidbody2DComponent>();
				auto  type				= spriteComponent.Type;
				auto  fixedRotation		= spriteComponent.FixedRotation;

				out << YAML::Key << "Type"			<< YAML::Value << (int)type;
				out << YAML::Key << "FixedRotation" << YAML::Value << fixedRotation;
			
				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;
			{
				auto& spriteComponent	= entity.GetComponent<BoxCollider2DComponent>();
				auto& offset			= spriteComponent.Offset;
				auto& size				= spriteComponent.Size;
				auto  density			= spriteComponent.Density;
				auto  friction			= spriteComponent.Friction;
				auto  restitution		= spriteComponent.Restitution;

				out << YAML::Key << "Offset"		<< YAML::Value << offset;
				out << YAML::Key << "Size"			<< YAML::Value << size;
				out << YAML::Key << "Density"		<< YAML::Value << density;
				out << YAML::Key << "Friction"		<< YAML::Value << friction;
				out << YAML::Key << "Restitution"	<< YAML::Value << restitution;

				out << YAML::EndMap;
			}
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << filepath.stem().string();

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		mScene->mRegistry.view<entt::entity>().each([&](auto entity_id)
			{
				Entity entity = { entity_id, mScene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
	{
		SORA_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		
		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		SORA_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint32_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				SORA_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = mScene->CreateEntity(name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& componet			= deserializedEntity.GetComponent<TransformComponent>();
					componet.Translation	= transformComponent["Translation"].as<glm::vec3>();
					componet.Rotation		= transformComponent["Rotation"].as<glm::vec3>();
					componet.Scale			= transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& component = deserializedEntity.AddComponent<CameraComponent>();

					auto cameraProps = cameraComponent["Camera"];
					component.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
				
					component.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					component.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					component.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					component.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					component.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					component.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					component.Primary			= cameraComponent["Primary"].as<bool>();
					component.FixedAspectRatio	= cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteComponent = entity["SpriteComponent"];
				if (spriteComponent)
				{
					auto& component		= deserializedEntity.AddComponent<SpriteComponent>();

					component.Color		= spriteComponent["Color"].as<glm::vec4>();
					if(spriteComponent["Texture"])
						component.Texture	= Texture2D::Create(spriteComponent["Texture"].as<std::string>());
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& component			= deserializedEntity.AddComponent<Rigidbody2DComponent>();
					component.Type			= (Rigidbody2DComponent::BodyType)rigidbody2DComponent["Type"].as<int>();
					component.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& component			= deserializedEntity.AddComponent<BoxCollider2DComponent>();
					component.Offset		= boxCollider2DComponent["Offset"].as<glm::vec2>();
					component.Size			= boxCollider2DComponent["Size"].as<glm::vec2>();
					component.Density		= boxCollider2DComponent["Density"].as<float>();
					component.Friction		= boxCollider2DComponent["Friction"].as<float>();
					component.Restitution	= boxCollider2DComponent["Restitution"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
	{
		SORA_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}