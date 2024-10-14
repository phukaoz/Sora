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
    
    /*
        
    */
    template<typename T = float>
    T GetValue(YAML::Node& node, const std::string& key, T optional = T())
    {
        if (node[key])
            return node[key].as<T>();

        return optional;
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
                auto& translation        = transformComponent.Translation;
                auto& rotation           = transformComponent.Rotation;
                auto& scale              = transformComponent.Scale;

                out << YAML::Key << "Translation" << YAML::Value << translation;
                out << YAML::Key << "Rotation"    << YAML::Value << rotation;
                out << YAML::Key << "Scale"       << YAML::Value << scale;
            
                out << YAML::EndMap;
            }
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap;
            {
                auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
                auto& color   = spriteRendererComponent.Color;
                auto& texture = spriteRendererComponent.Texture;

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
                auto& color     = circleRendererComponent.Color;
                float thickness = circleRendererComponent.Thickness;
                float fade      = circleRendererComponent.Fade;

                out << YAML::Key << "Color"     << YAML::Value << color;
                out << YAML::Key << "Thickness" << YAML::Value << thickness;
                out << YAML::Key << "Fade"      << YAML::Value << fade;

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
                    out << YAML::Key << "ProjectionType"   << YAML::Value << (int)camera.GetProjectionType();

                    out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
                    out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
                    out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.GetOrthographicFarClip();

                    out << YAML::Key << "PerspectiveFOV"   << YAML::Value << camera.GetPerspectiveVerticalFOV();
                    out << YAML::Key << "PerspectiveNear"  << YAML::Value << camera.GetPerspectiveNearClip();
                    out << YAML::Key << "PerspectiveFar"   << YAML::Value << camera.GetPerspectiveFarClip();

                    out << YAML::EndMap;
                }

                out << YAML::Key << "Primary"          << YAML::Value << cameraComponent.Primary;
                out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

                out << YAML::EndMap;
            }
        }

        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            out << YAML::Key << "Rigidbody2DComponent";
            out << YAML::BeginMap;
            {
                auto& rigidbody2DComponent = entity.GetComponent<Rigidbody2DComponent>();
                auto  type                 = rigidbody2DComponent.Type;
                auto  fixedRotation        = rigidbody2DComponent.FixedRotation;

                out << YAML::Key << "Type"          << YAML::Value << (int)type;
                out << YAML::Key << "FixedRotation" << YAML::Value << fixedRotation;
            
                out << YAML::EndMap;
            }
        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap;
            {
                auto& boxColiider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
                auto& offset      = boxColiider2DComponent.Offset;
                auto& size        = boxColiider2DComponent.Size;
                auto  density     = boxColiider2DComponent.Density;
                auto  friction    = boxColiider2DComponent.Friction;
                auto  restitution = boxColiider2DComponent.Restitution;

                out << YAML::Key << "Offset"      << YAML::Value << offset;
                out << YAML::Key << "Size"        << YAML::Value << size;
                out << YAML::Key << "Density"     << YAML::Value << density;
                out << YAML::Key << "Friction"    << YAML::Value << friction;
                out << YAML::Key << "Restitution" << YAML::Value << restitution;

                out << YAML::EndMap;
            }
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            out << YAML::Key << "CircleCollider2DComponent";
            out << YAML::BeginMap;
            {
                auto& circleColiider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
                auto& offset      = circleColiider2DComponent.Offset;
                auto& radius      = circleColiider2DComponent.Radius;
                auto  density     = circleColiider2DComponent.Density;
                auto  friction    = circleColiider2DComponent.Friction;
                auto  restitution = circleColiider2DComponent.Restitution;

                out << YAML::Key << "Offset"      << YAML::Value << offset;
                out << YAML::Key << "Radius"      << YAML::Value << radius;
                out << YAML::Key << "Density"     << YAML::Value << density;
                out << YAML::Key << "Friction"    << YAML::Value << friction;
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
                out << YAML::Key << "FOV"         << YAML::Value << editorCamera.GetFOV();
                out << YAML::Key << "AspectRatio" << YAML::Value << editorCamera.GetAspectRatio();
                out << YAML::Key << "NearClip"    << YAML::Value << editorCamera.GetNearClip();
                out << YAML::Key << "FarClip"     << YAML::Value << editorCamera.GetFarClip();
                out << YAML::Key << "FocalPoint"  << YAML::Value << editorCamera.GetFocalPoint();
                out << YAML::Key << "Distance"    << YAML::Value << editorCamera.GetDistance();
                out << YAML::Key << "Pitch"       << YAML::Value << editorCamera.GetPitch();
                out << YAML::Key << "Yaw"         << YAML::Value << editorCamera.GetYaw();

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

        std::string sceneName = GetValue<std::string>(data, "Scene");
        SORA_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = GetValue<uint64_t>(entity, "Entity");

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = GetValue<std::string>(tagComponent, "Tag");

                SORA_CORE_TRACE("Deserialized entity '{0}' ID: {1}", name, uuid);

                Entity deserializedEntity = m_Scene->CreateEntity(name, uuid);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& componet       = deserializedEntity.GetComponent<TransformComponent>();
                    componet.Translation = GetValue<glm::vec3>(transformComponent, "Translation");
                    componet.Rotation    = GetValue<glm::vec3>(transformComponent, "Rotation");
                    componet.Scale       = GetValue<glm::vec3>(transformComponent, "Scale");
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& component = deserializedEntity.AddComponent<CameraComponent>();

                    auto cameraProps = cameraComponent["Camera"];
                    if (cameraProps)
                    {
                        component.Camera.SetProjectionType((SceneCamera::ProjectionType)GetValue<int>(cameraProps, "ProjectionType"));

                        component.Camera.SetOrthographicSize      (GetValue<float>(cameraProps, "OrthographicSize"));
                        component.Camera.SetOrthographicNearClip  (GetValue<float>(cameraProps, "OrthographicNear"));
                        component.Camera.SetOrthographicFarClip   (GetValue<float>(cameraProps, "OrthographicFar"));

                        component.Camera.SetPerspectiveVerticalFOV(GetValue<float>(cameraProps, "PerspectiveFOV"));
                        component.Camera.SetPerspectiveNearClip   (GetValue<float>(cameraProps, "PerspectiveNear"));
                        component.Camera.SetPerspectiveFarClip    (GetValue<float>(cameraProps, "PerspectiveFar"));
                    }

                    component.Primary          = GetValue<bool>(cameraComponent, "Primary");
                    component.FixedAspectRatio = GetValue<bool>(cameraComponent, "FixedAspectRatio");
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if (spriteRendererComponent)
                {
                    auto& component = deserializedEntity.AddComponent<SpriteRendererComponent>();

                    component.Color = GetValue<glm::vec4>(spriteRendererComponent, "Color");
                    if(spriteRendererComponent["Texture"])
                        component.Texture = Texture2D::Create(GetValue<std::string>(spriteRendererComponent, "Texture"));
                }

                auto circleRendererComponent = entity["CircleRendererComponent"];
                if (circleRendererComponent)
                {
                    auto& component	    = deserializedEntity.AddComponent<CircleRendererComponent>();
                    component.Color     = GetValue<glm::vec4>(circleRendererComponent, "Color");
                    component.Thickness = GetValue<float>    (circleRendererComponent, "Thickness");
                    component.Fade      = GetValue<float>    (circleRendererComponent, "Fade");
                }

                auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
                if (rigidbody2DComponent)
                {
                    auto& component			= deserializedEntity.AddComponent<Rigidbody2DComponent>();
                    component.Type			= (Rigidbody2DComponent::BodyType)GetValue<int>(rigidbody2DComponent, "Type");
                    component.FixedRotation = GetValue<bool>(rigidbody2DComponent, "FixedRotation");
                }

                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
                if (boxCollider2DComponent)
                {
                    auto& component       = deserializedEntity.AddComponent<BoxCollider2DComponent>();
                    component.Offset      = GetValue<glm::vec2>(boxCollider2DComponent, "Offset");
                    component.Size        = GetValue<glm::vec2>(boxCollider2DComponent, "Size");
                    component.Density     = GetValue<float>    (boxCollider2DComponent, "Density");
                    component.Friction    = GetValue<float>    (boxCollider2DComponent, "Friction");
                    component.Restitution = GetValue<float>    (boxCollider2DComponent, "Restitution");
                }

                auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
                if (circleCollider2DComponent)
                {
                    auto& component	      = deserializedEntity.AddComponent<CircleCollider2DComponent>();
                    component.Offset      = GetValue<glm::vec2>(circleCollider2DComponent, "Offset");
                    component.Radius      = GetValue<float>    (circleCollider2DComponent, "Radius");
                    component.Density     = GetValue<float>    (circleCollider2DComponent, "Density");
                    component.Friction    = GetValue<float>    (circleCollider2DComponent, "Friction");
                    component.Restitution = GetValue<float>    (circleCollider2DComponent, "Restitution");
                }
            }
        }

        auto editorCamera = data["Camera"];
        if (editorCamera)
        {
            float fov            = GetValue<float>    (editorCamera, "FOV");
            float aspectRatio    = GetValue<float>    (editorCamera, "AspectRatio");
            float nearClip       = GetValue<float>    (editorCamera, "NearClip");
            float farClip        = GetValue<float>    (editorCamera, "FarClip");
            m_Scene->GetEditorCamera() = EditorCamera(fov, aspectRatio, nearClip, farClip);
            
            float distance       = GetValue<float>    (editorCamera, "Distance");
            float pitch	         = GetValue<float>    (editorCamera, "Pitch");
            float yaw            = GetValue<float>    (editorCamera, "Yaw");
            glm::vec3 focalPoint = GetValue<glm::vec3>(editorCamera, "FocalPoint");

            m_Scene->GetEditorCamera().SetDistance(distance);
            m_Scene->GetEditorCamera().SetPitch(pitch);
            m_Scene->GetEditorCamera().SetYaw(yaw);
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