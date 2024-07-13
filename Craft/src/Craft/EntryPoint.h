#pragma once

#ifdef CRAFT_PLATFORM_WINDOWS

extern Craft::Application* Craft::CreateApplication();

int main(int argc, char** argv) {
	auto app = Craft::CreateApplication();
	app->Run();
	delete app;
}

#endif