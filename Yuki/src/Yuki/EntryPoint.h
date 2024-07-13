#pragma once

#ifdef YUKI_PLATFORM_WINDOWS

extern Yuki::Application* Yuki::CreateApplication();

int main(int argc, char** argv) {
	auto app = Yuki::CreateApplication();
	app->Run();
	delete app;
}

#endif