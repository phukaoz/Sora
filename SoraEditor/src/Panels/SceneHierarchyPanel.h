#pragma once

#include "Sora.h"

namespace Sora {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);
	private:
		Ref<Scene> m_Context;

		Entity m_SelectionContext;
	};

}