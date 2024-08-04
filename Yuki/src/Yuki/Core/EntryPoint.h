#pragma once

#ifdef YUKI_PLATFORM_WINDOWS

extern Yuki::Application* Yuki::CreateApplication();

int main(int argc, char** argv) 
{
	Yuki::Log::Init();

	auto app = Yuki::CreateApplication();
	app->Run();
	delete app;
}

#endif