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
		: m_Scene(scene)
	{

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		SORA_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity doesn't have an id!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		
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
				auto& translation		= transformComponent.Translation;
				auto& rotation			= transformComponent.Rotation;
				auto& scale				= transformComponent.Scale;

				out << YAML::Key << "Translation"	<< YAML::Value << translation;
				out << YAML::Key << "Rotation"		<< YAML::Value << rotation;
				out << YAML::Key << "Scale"			<< YAML::Value << scale;
			
				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			{
				auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
				auto& color		= spriteRendererComponent.Color;
				auto& texture	= spriteRendererComponent.Texture;

				out << YAML::Key << "Color"	<< YAML::Value << color;
				if (texture.get())
					out << YAML::Key << "Texture" << YAML::Value << texture->GetTexturePath().string();

				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;
			{
				auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
				auto& color		= circleRendererComponent.Color;
				float thickness = circleRendererComponent.Thickness;
				float fade		= circleRendererComponent.Fade;

				out << YAML::Key << "Color" << YAML::Value << color;
				out << YAML::Key << "Thickness" << YAML::Value << thickness;
				out << YAML::Key << "Fade" << YAML::Value << fade;

				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			{
				auto& cameraComponent	= entity.GetComponent<CameraComponent>();
				auto& camera			= cameraComponent.Camera;
				auto& primary			= cameraComponent.Primary;
				auto& fixedAspectRatio	= cameraComponent.FixedAspectRatio;

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
				auto& rigidbody2DComponent	= entity.GetComponent<Rigidbody2DComponent>();
				auto  type					= rigidbody2DComponent.Type;
				auto  fixedRotation			= rigidbody2DComponent.FixedRotation;

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
				auto& boxColiider2DComponent	= entity.GetComponent<BoxCollider2DComponent>();
				auto& offset		= boxColiider2DComponent.Offset;
				auto& size			= boxColiider2DComponent.Size;
				auto  density		= boxColiider2DComponent.Density;
				auto  friction		= boxColiider2DComponent.Friction;
				auto  restitution	= boxColiider2DComponent.Restitution;

				out << YAML::Key << "Offset"		<< YAML::Value << offset;
				out << YAML::Key << "Size"			<< YAML::Value << size;
				out << YAML::Key << "Density"		<< YAML::Value << density;
				out << YAML::Key << "Friction"		<< YAML::Value << friction;
				out << YAML::Key << "Restitution"	<< YAML::Value << restitution;

				out << YAML::EndMap;
			}
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;
			{
				auto& circleColiider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
				auto& offset		= circleColiider2DComponent.Offset;
				auto& radius		= circleColiider2DComponent.Radius;
				auto  density		= circleColiider2DComponent.Density;
				auto  friction		= circleColiider2DComponent.Friction;
				auto  restitution	= circleColiider2DComponent.Restitution;

				out << YAML::Key << "Offset" << YAML::Value << offset;
				out << YAML::Key << "Radius" << YAML::Value << radius;
				out << YAML::Key << "Density" << YAML::Value << density;
				out << YAML::Key << "Friction" << YAML::Value << friction;
				out << YAML::Key << "Restitution" << YAML::Value << restitution;

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
		{
			m_Scene->m_Registry.view<entt::entity>().each([&](auto entity_id)
				{
					Entity entity = { entity_id, m_Scene.get() };
					if (!entity)
						return;

					SerializeEntity(out, entity);
				});

			out << YAML::EndSeq;
		}
		
		// Editor Camera
		out << YAML::Key << "Camera" << YAML::Value;
		{
			out << YAML::BeginMap;
			{
				auto& editorCamera = m_Scene->GetEditorCamera();
				out << YAML::Key << "FOV"			<< YAML::Value << editorCamera.GetFOV();
				out << YAML::Key << "AspectRatio"	<< YAML::Value << editorCamera.GetAspectRatio();
				out << YAML::Key << "NearClip"		<< YAML::Value << editorCamera.GetNearClip();
				out << YAML::Key << "FarClip"		<< YAML::Value << editorCamera.GetFarClip();
				out << YAML::Key << "Position"		<< YAML::Value << editorCamera.GetPosition();
				out << YAML::Key << "FocalPoint"	<< YAML::Value << editorCamera.GetFocalPoint();
				out << YAML::Key << "Distance"		<< YAML::Value << editorCamera.GetDistance();
				out << YAML::Key << "Pitch"			<< YAML::Value << editorCamera.GetPitch();
				out << YAML::Key << "Yaw"			<< YAML::Value << editorCamera.GetYaw();

				out << YAML::EndMap;
			}
		}

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
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				SORA_CORE_TRACE("Deserialized entity '{0}' ID: {1}", name, uuid);

				Entity deserializedEntity = m_Scene->CreateEntity(name, uuid);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& componet			= deserializedEntity.GetComponent<TransformComponent>();
					componet.Translation	= transformComponent["Translation"]	.as<glm::vec3>();
					componet.Rotation		= transformComponent["Rotation"]	.as<glm::vec3>();
					componet.Scale			= transformComponent["Scale"]		.as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& component = deserializedEntity.AddComponent<CameraComponent>();

					auto cameraProps = cameraComponent["Camera"];
					component.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
				
					component.Camera.SetOrthographicSize(cameraProps["OrthographicSize"]	.as<float>());
					component.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					component.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"]	.as<float>());

					component.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					component.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"]	.as<float>());
					component.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"]	.as<float>());

					component.Primary			= cameraComponent["Primary"]			.as<bool>();
					component.FixedAspectRatio	= cameraComponent["FixedAspectRatio"]	.as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& component		= deserializedEntity.AddComponent<SpriteRendererComponent>();

					component.Color		= spriteRendererComponent["Color"].as<glm::vec4>();
					if(spriteRendererComponent["Texture"])
						component.Texture	= Texture2D::Create(spriteRendererComponent["Texture"].as<std::string>());
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& component		= deserializedEntity.AddComponent<CircleRendererComponent>();

					component.Color		= circleRendererComponent["Color"]		.as<glm::vec4>();
					component.Thickness = circleRendererComponent["Thickness"]	.as<float>();
					component.Fade		= circleRendererComponent["Fade"]		.as<float>();
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
					component.Offset		= boxCollider2DComponent["Offset"]		.as<glm::vec2>();
					component.Size			= boxCollider2DComponent["Size"]		.as<glm::vec2>();
					component.Density		= boxCollider2DComponent["Density"]		.as<float>();
					component.Friction		= boxCollider2DComponent["Friction"]	.as<float>();
					component.Restitution	= boxCollider2DComponent["Restitution"]	.as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& component			= deserializedEntity.AddComponent<CircleCollider2DComponent>();
					component.Offset		= circleCollider2DComponent["Offset"]		.as<glm::vec2>();
					component.Radius		= circleCollider2DComponent["Radius"]		.as<float>();
					component.Density		= circleCollider2DComponent["Density"]		.as<float>();
					component.Friction		= circleCollider2DComponent["Friction"]		.as<float>();
					component.Restitution	= circleCollider2DComponent["Restitution"]	.as<float>();
				}
			}
		}

		auto editorCamera = data["Camera"];
		if (editorCamera)
		{
			float fov			 = editorCamera["FOV"]			.as<float>();
			float aspectRatio	 = editorCamera["AspectRatio"]	.as<float>();
			float nearClip		 = editorCamera["NearClip"]		.as<float>();
			float farClip		 = editorCamera["FarClip"]		.as<float>();
			m_Scene->GetEditorCamera() = EditorCamera(fov, aspectRatio, nearClip, farClip);
			
			float distance		 = editorCamera["Distance"]		.as<float>();
			float pitch			 = editorCamera["Pitch"]		.as<float>();
			float yaw			 = editorCamera["Yaw"]			.as<float>();
			glm::vec3 position	 = editorCamera["Position"]		.as<glm::vec3>();
			glm::vec3 focalPoint = editorCamera["FocalPoint"]	.as<glm::vec3>();

			m_Scene->GetEditorCamera().SetDistance(distance);
			m_Scene->GetEditorCamera().SetPitch(pitch);
			m_Scene->GetEditorCamera().SetYaw(yaw);
			m_Scene->GetEditorCamera().SetPosition(position);
			m_Scene->GetEditorCamera().SetFocalPoint(focalPoint);
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
	{
		SORA_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}