
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

std::vector<double> expensive()
{
    double a = 0.99999999999;
    int N = 20000;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
        {
            a += 2;
        }
    return {a, a / 10., a * 10.};
}

int main()
{
    auto vec = expensive();
    for (auto &v : vec)
        std::cout << v << std::endl;
    return 0;
}