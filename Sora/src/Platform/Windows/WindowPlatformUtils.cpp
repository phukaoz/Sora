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
		OPENFILENAMEA open_filename;
		CHAR size_file[256] = { 0 };
		CHAR current_dir[256] = { 0 };
		ZeroMemory(&open_filename, sizeof(open_filename));
		open_filename.lStructSize = sizeof(open_filename);
		open_filename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		open_filename.lpstrFile = size_file;
		open_filename.nMaxFile = sizeof(size_file);
		if (GetCurrentDirectoryA(256, current_dir))
			open_filename.lpstrInitialDir = current_dir;

		open_filename.lpstrFilter = filter;
		open_filename.nFilterIndex = 1;
		open_filename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&open_filename) == TRUE)
			return open_filename.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA open_filename;
		CHAR size_file[256] = { 0 };
		CHAR current_dir[256] = { 0 };
		ZeroMemory(&open_filename, sizeof(open_filename));
		open_filename.lStructSize = sizeof(open_filename);
		open_filename.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		open_filename.lpstrFile = size_file;
		open_filename.nMaxFile = sizeof(size_file);
		if (GetCurrentDirectoryA(256, current_dir))
			open_filename.lpstrInitialDir = current_dir;

		open_filename.lpstrFilter = filter;
		open_filename.nFilterIndex = 1;
		open_filename.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&open_filename) == TRUE)
			return open_filename.lpstrFile;

		return std::string();
	}

}