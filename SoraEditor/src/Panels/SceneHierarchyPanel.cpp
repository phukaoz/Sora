#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Sora {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy"); 
		
		auto view = m_Context->m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			DrawEntityNode(Entity(entity, m_Context.get()));
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_Context->CreateEntity();

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) 
			| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth ;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		
		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::SeparatorText(tag.c_str());
			if (ImGui::MenuItem("Delete Entity"))
				entity_deleted = true;

			ImGui::EndPopup();
		}
		
		if (opened)
		{
			ImGui::TreePop();
		}

		if (entity_deleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float column_width = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto bold_font = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 button_size = { line_height + 3.0f, line_height };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.0f, 0.05f, 1.0f });
		ImGui::PushFont(bold_font);
		if (ImGui::Button("X", button_size))
			values.x = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.8f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.9f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.7f, 0.05f, 1.0f });
		ImGui::PushFont(bold_font);
		if (ImGui::Button("Y", button_size))
			values.y = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.25f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.05f, 0.7f, 1.0f });
		ImGui::PushFont(bold_font);
		if (ImGui::Button("Z", button_size))
			values.z = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename ComponentType, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction ui_function)
	{
		const ImGuiTreeNodeFlags TREE_NODE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<ComponentType>())
		{
			auto& component = entity.GetComponent<ComponentType>();
			ImVec2 content_region_available = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool opened = ImGui::TreeNodeEx((void*)typeid(ComponentType).hash_code(), TREE_NODE_FLAGS, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(content_region_available.x - line_height * 0.5f);

			if (ImGui::Button("+", ImVec2{ line_height, line_height }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removed = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removed = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				ui_function(component);
				ImGui::TreePop();
			}

			if (removed)
				entity.RemoveComponent<ComponentType>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if(ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		{
			ImGui::SameLine();
			ImGui::PushItemWidth(-1);

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Sprite"))
				{
					m_SelectionContext.AddComponent<SpriteComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::PopItemWidth();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				const char* projection_type_string[] = { "Perspective", "Orthographic" };
				const char* current_projection_type_string = projection_type_string[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", current_projection_type_string))
				{
					for (int i = 0; i < 2; i++)
					{
						bool is_selected = current_projection_type_string == projection_type_string[i];
						if (ImGui::Selectable(projection_type_string[i], is_selected))
						{
							current_projection_type_string = projection_type_string[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float vertical_FOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("FOV", &vertical_FOV))
						camera.SetPerspectiveVerticalFOV(glm::radians(vertical_FOV));

					float vertical_near = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &vertical_near))
						camera.SetPerspectiveNearClip(vertical_near);

					float vertical_far = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &vertical_far))
						camera.SetPerspectiveFarClip(vertical_far);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float ortho_size = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &ortho_size))
						camera.SetOrthographicSize(ortho_size);

					float ortho_near = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &ortho_near))
						camera.SetOrthographicNearClip(ortho_near);

					float ortho_far = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &ortho_far))
						camera.SetOrthographicFarClip(ortho_far);

					if (ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio))
					{
						//TODO: recalculate scene camera
					}

					if (ImGui::Checkbox("Primary", &component.Primary))
					{

					}
				}
			});
	
		DrawComponent<SpriteComponent>("Sprite", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			});
	}

}