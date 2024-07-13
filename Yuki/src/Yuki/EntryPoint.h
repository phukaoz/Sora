#pragma once

#ifdef YUKI_PLATFORM_WINDOWS

extern Yuki::Application* Yuki::CreateApplication();

int main(int argc, char** argv) {
	Yuki::Log::Init();
	int a = 10;
	YUKI_CORE_INFO("my var = {0}", a);

	auto app = Yuki::CreateApplication();
	app->Run();
	delete app;
}

#endif