// #include <thrust/host_vector.h>
// #include <thrust/device_vector.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <Kokkos_Core.hpp>

class A
{
public:
	int a;
	KOKKOS_FUNCTION A(int b): a(b) {}
};

// KOKKOS_FUNCTION A func(int a)
KOKKOS_FUNCTION A func(int a)
{
	// thrust::host_vector<int> H(4);
	// thrust::device_vector<int> D = H;
	// thrust::device_vector<int> D(4);
	// std::vector<double> vec(5);
	// Kokkos::View<double*> view("view", 5);
	double vec[4] = {0.,0.,0.,0.};
	return A(a + 2);
}

std::vector<double> expensive()
{
	double a = 0.99999999999;
	int N = 100000;

	Kokkos::parallel_reduce(N, KOKKOS_LAMBDA(int i, double & update) {
		for (int j = 0; j < N; ++j)
			update += func(0).a;
	}, a);

	return {a, a / 10., a * 10.};
}

int main()
{
	Kokkos::initialize();
	{
		auto vec = expensive();

		for (auto& v : vec)
			std::cout << v << std::endl;

		std::cout << A(2).a << std::endl;
	}
	Kokkos::finalize();

	return 0;
}