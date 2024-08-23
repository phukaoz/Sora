#include <Yuki.h>
#include <Yuki/Core/EntryPoint.h>

#include "AlyaLayer.h"

namespace Yuki {

	class Alya : public Application
	{
	public:
		Alya()
			: Application("Alya")
		{
			PushLayer(new AlyaLayer());
		}

		~Alya()
		{

		}
	};

	Application* CreateApplication()
	{
		return new Alya();
	}

}