#pragma once
#include "Sora/Core/Core.h"
#include "Sora/Core/Application.h"

#ifdef SORA_PLATFORM_WINDOWS

extern Sora::Application* Sora::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) 
{
	Sora::Log::Init();

	SORA_PROFILE_BEGIN_SESSION("Startup", "SoraProfile-Startup.json");
	auto app = Sora::CreateApplication({ argc, argv });
	SORA_PROFILE_END_SESSION();

	SORA_PROFILE_BEGIN_SESSION("Runtime", "SoraProfile-Runtime.json");
	app->Run();
	SORA_PROFILE_END_SESSION();

	SORA_PROFILE_BEGIN_SESSION("Shutdown", "SoraProfile-Shutdown.json");
	delete app;
	SORA_PROFILE_END_SESSION();
}

#endif