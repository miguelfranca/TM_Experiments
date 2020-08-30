
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#ifdef KOKKOS
	#include <Kokkos_Core.hpp>
#endif

#include "expensive.hpp"

std::vector<double> expensive()
{
	double a = 0.99999999999;
	int N = 100000;

#ifdef KOKKOS
	Kokkos::parallel_reduce(N, KOKKOS_LAMBDA(int i, double & update) {
		for (int j = 0; j < N; ++j)
			update += 2;
	}, a);
#else

	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			a += 2;

#endif

	return {a, a / 10., a * 10.};
}

int main()
{
#ifdef KOKKOS
	Kokkos::initialize();
	{
#endif

		auto vec = expensive();

		for (auto& v : vec)
			std::cout << v << std::endl;

#ifdef KOKKOS
	}
	Kokkos::finalize();
#endif

	return 0;
}