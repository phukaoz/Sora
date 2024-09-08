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
			static float thumbnail_size = 128.0f;
			float cell_size = thumbnail_size + padding;

			float panel_width = ImGui::GetContentRegionAvail().x;
			int column_count = (int)(panel_width / cell_size);
			if (column_count < 1)
				column_count = 1;

			ImGui::Columns(column_count, 0, false);

			for (auto& directory : std::filesystem::directory_iterator(mCurrentDirectory))
			{
				const auto& path = directory.path();
				auto relative_path = std::filesystem::relative(directory.path(), gAssetPath);
				std::string filename = relative_path.filename().string();

				Ref<Texture2D> icon = directory.is_directory() ? mDirectoryIcon : mFileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(filename.c_str(), (ImTextureID)(uint64_t)icon->GetRendererID(), {thumbnail_size, thumbnail_size}, {0, 1}, {1, 0});
				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* item_path = relative_path.c_str();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", item_path, (wcslen(item_path) + 1) * sizeof(wchar_t), ImGuiCond_Once);

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