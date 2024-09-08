#include "sorapch.h"
#include "Sora/Utils/PlatformUtils.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Sora/Core/Application.h"

namespace Sora {

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA openFilename;
		CHAR sizeFile[256] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&openFilename, sizeof(openFilename));
		openFilename.lStructSize = sizeof(openFilename);
		openFilename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		openFilename.lpstrFile = sizeFile;
		openFilename.nMaxFile = sizeof(sizeFile);
		if (GetCurrentDirectoryA(256, currentDir))
			openFilename.lpstrInitialDir = currentDir;

		openFilename.lpstrFilter = filter;
		openFilename.nFilterIndex = 1;
		openFilename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&openFilename) == TRUE)
			return openFilename.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA openFilename;
		CHAR size_file[256] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&openFilename, sizeof(openFilename));
		openFilename.lStructSize = sizeof(openFilename);
		openFilename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		openFilename.lpstrFile = size_file;
		openFilename.nMaxFile = sizeof(size_file);
		if (GetCurrentDirectoryA(256, currentDir))
			openFilename.lpstrInitialDir = currentDir;

		openFilename.lpstrFilter = filter;
		openFilename.nFilterIndex = 1;
		openFilename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&openFilename) == TRUE)
		{
			std::string filePath = openFilename.lpstrFile;

			std::string fileExtension;
			const char* extPos = strchr(filter, '\0') + 1;
			if (extPos && strlen(extPos) > 0)
			{
				fileExtension = extPos;
				size_t wildcardPos = fileExtension.find('*');
				if (wildcardPos != std::string::npos)
					fileExtension = fileExtension.substr(wildcardPos + 1);
			}

			if (!fileExtension.empty() && filePath.find('.') == std::string::npos)
				filePath += fileExtension;

			return filePath;
		}


		return std::string();
	}

}