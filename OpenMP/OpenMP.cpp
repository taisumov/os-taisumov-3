#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

#define BLOCKSIZE 830819
#define N 100000000

int main()
{

    system("chcp 1251");

    int counterThreads;
    long double inversedN = 1.0 / N;
    long double pi = 0.0;
    long double x_i;

    system("cls");

    std::cout << "Введите количество потоков: ";
    std::cin >> counterThreads;

    DWORD startTime = timeGetTime();

    #pragma omp parallel for schedule(dynamic,BLOCKSIZE) private(x_i) num_threads(counterThreads) reduction (+:pi)

    for (long long i = 0; i < N; i++)
    {
        x_i = (i + 0.5) * inversedN;
        pi = pi + 4.0 / (1.0 + x_i * x_i);
    }

    pi *= inversedN;
    DWORD endTime = timeGetTime();

    std::cout << "Время вычислений: " << endTime - startTime << std::endl;
    std::cout << "Результат вычислений: " << std::fixed << std::setprecision(10) << pi << std::endl;

    return 0;
}