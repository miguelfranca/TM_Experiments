
#ifndef KOKKOS
#define KOKKOS_FUNCTION
#include <vector>
typedef VecH std::vector<double>
typedef VecD std::vector<double>
typedef MatrixH std::vector<std::vector<double>>
typedef MatrixD std::vector<std::vector<double>>
#else
#include <Kokkos_Core.hpp>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
typedef VecH thrust::host_vector<double>
typedef VecD thrust::device_vector<double>
typedef MatrixH thrust::host_vector<thrust::host_vector<double>>
typedef MatrixD thrust::device_vector<thrust::device_vector<double>>
#endif