#include "Application.h"

#include <Kokkos_Core.hpp>

int main()
{
#ifdef KOKKOS
	Kokkos::initialize();
	{
#endif
		Application app;
		app.showFPS(false);
		app.setMaxFPS(120);
		app.run();

#ifdef KOKKOS
	}
	Kokkos::finalize();
#endif

	return 0;
}
