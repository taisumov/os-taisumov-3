#include <windows.h>
#include <iostream>

#define N 100000000
#define BLOCKSIZE 830819

using namespace std;

const double inversedN = 1.0 / N;
int counterThreads;

//Структура, доступ к которой получают потоки

typedef struct
{
    volatile long* const counter;   //Указатель на счётчик - волатильная переменная
    double pi;                      //Число Пи, вычисляемое потоками

} THREADPI;


DWORD WINAPI Calculation(LPVOID lpData) {

    THREADPI* Var = (THREADPI*)lpData;

    bool flag = true;
    int start = 0;
    int end = 0;
    double result = 0;
    double sum = 0;
    double x_i = 0;
    long block_num = 0;

    while (flag)
    {
        //Инициализация

        block_num = InterlockedIncrement(Var->counter) - 1; //Инкрементирование волатильной переменной и переменной функции
        start = BLOCKSIZE * block_num;                      //Начало текущего блока
        end   = BLOCKSIZE * (block_num + 1);                //Конец текущего блока

        //Вычисление числа Пи

        if (start < N)  
        {
            sum = 0;
            x_i = 0;

            for (int i = start; (i < N) && (i < end); ++i)
            {
                x_i = (i + 0.5) * inversedN;
                sum += 4 / (1 + x_i * x_i);
            }

            result += sum;
        }
        else
        {
            flag = !flag;   //Флаг, определяющий конец вычислений
        }
    }

    Var->pi = result;   

    return 0;

}

int main()
{
    system("chcp 1251");
    system("cls");

    cout << "Введите количество потоков: ";
    cin >> counterThreads;

    THREADPI** Var = new THREADPI * [counterThreads];   //Массив, содержащий структуры, используемые для вычисления числа Пи
    HANDLE* Threads = new HANDLE[counterThreads];      //Массив, содержащий дескрипторы потоков
    DWORD* ThreadsID = new DWORD[counterThreads];       //Массив, содержащий ID потоков

    long* counter = new long;
    *counter = 0;

    //Создание массива потоков с присваиванием каждому потоку собственного ID (ID хранятся в отдельном массиве)

    for (int i = 0; i < counterThreads; ++i)
    {
        Var[i] = new THREADPI{ counter, 0 };

        Threads[i] = CreateThread(NULL, 0, Calculation, Var[i],
            CREATE_SUSPENDED, &ThreadsID[i]);
    }

    //Начало измерения времени работы
    unsigned int startTime = GetTickCount();

    for (int i = 0; i < counterThreads; ++i)
    {
        ResumeThread(Threads[i]);
    }

    WaitForMultipleObjects(counterThreads, Threads, TRUE, INFINITE);    //Ожидание изменения состояния потоков

    double sum = 0;

    for (int i = 0; i < counterThreads; ++i)
    {
        sum += Var[i]->pi;
    }

    sum = sum * inversedN;

    //Конец измерения времени работы
    unsigned int endTime = GetTickCount();

    cout << "Время вычислений: " << endTime - startTime << '\n';
    cout.precision(10);
    cout << "Результат вычислений: " << sum << '\n';

    for (int i = 0; i < counterThreads; ++i)
    {
        CloseHandle(Threads[i]);
    }
    delete[] Var;

    return 0;
}
