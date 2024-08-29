#include <Sora.h>
#include <Sora/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Sora {

	class SoraEditor : public Application
	{
	public:
		SoraEditor()
			: Application("Sora Editor")
		{
			PushLayer(new EditorLayer());
		}

		~SoraEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new SoraEditor();
	}

}