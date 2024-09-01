#include <Sora.h>
#include <Sora/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Sora {

	class SoraEditor : public Application
	{
	public:
		SoraEditor(ApplicationCommandLineArgs args)
			: Application("Sora Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~SoraEditor()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new SoraEditor(args);
	}

}