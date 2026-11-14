#include <iostream>
#include <windows.h>

using namespace std;


bool isPrime(int n)
{
    if (n < 2)
    {
        return false;
    }

    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");

    if (argc < 3)
    {
        cout << "Не переданы аргументы диапазона" << endl;
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    int count = 0;

    //поиск
    for (int i = start; i <= end; i++)
    {
        if (isPrime(i))
        {
            count++;
        }
    }

    cout << "Вычисления завершены. Найдено простых чисел: " << count << endl;

    return 0;
}