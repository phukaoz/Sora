#pragma once

#ifdef YUKI_PLATFORM_WINDOWS

extern Yuki::Application* Yuki::CreateApplication();

int main(int argc, char** argv) 
{
	Yuki::Log::Init();

	YUKI_PROFILE_BEGIN_SESSION("Startup", "YukiProfile-Startup.json");
	auto app = Yuki::CreateApplication();
	YUKI_PROFILE_END_SESSION();

	YUKI_PROFILE_BEGIN_SESSION("Runtime", "YukiProfile-Runtime.json");
	app->Run();
	YUKI_PROFILE_END_SESSION();

	YUKI_PROFILE_BEGIN_SESSION("Shutdown", "YukiProfile-Shutdown.json");
	delete app;
	YUKI_PROFILE_END_SESSION();
}

#endif