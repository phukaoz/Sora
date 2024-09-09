#include "sorapch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Sora {

	extern const std::filesystem::path gAssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: mCurrentDirectory(gAssetPath)
	{
		mDirectoryIcon = Texture2D::Create("resources/icons/ContentBrowser/DirectoryIcon.png");
		mFileIcon = Texture2D::Create("resources/icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Content Browser"))
		{
			if (mCurrentDirectory != std::filesystem::path(gAssetPath))
			{
				if (ImGui::Button("<-"))
				{
					mCurrentDirectory = mCurrentDirectory.parent_path();
				}
			}

			static float padding = 16.0f;
			static float thumbnailSize = 128.0f;
			float cellSize = thumbnailSize + padding;

			float panelWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = (int)(panelWidth / cellSize);
			if (columnCount < 1)
				columnCount = 1;

			ImGui::Columns(columnCount, 0, false);

			for (auto& directory : std::filesystem::directory_iterator(mCurrentDirectory))
			{
				const auto& path = directory.path();
				auto relativePath = std::filesystem::relative(directory.path(), gAssetPath);
				std::string filename = relativePath.filename().string();

				Ref<Texture2D> icon = directory.is_directory() ? mDirectoryIcon : mFileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(filename.c_str(), (ImTextureID)(uint64_t)icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);

					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directory.is_directory())
						mCurrentDirectory /= path.filename();
				}
				ImGui::TextWrapped(filename.c_str());

				ImGui::NextColumn();
			}

			ImGui::End();
		}

		
	}

}