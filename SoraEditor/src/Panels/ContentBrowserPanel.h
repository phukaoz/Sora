#pragma once

#include <filesystem>

#include "Sora/Renderer/Texture.h"

namespace Sora {
	
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path mCurrentDirectory;
	
		Ref<Texture2D> mDirectoryIcon, mFileIcon;
	};

}