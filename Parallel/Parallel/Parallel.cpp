// Parallel.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>

int const N = 512;
float const eps = 1.0;
float const h = 1.0 / N;
float X[N + 1];
float Y[N + 1];

float U[N + 1][N + 1];
float UPred[N + 1][N + 1];
float tempArr[N + 1][N + 1];

float f;
bool FidndingWasEnd;
float maxDiffInArr;

std::fstream out("ANSWER.txt");
std::fstream diffOut("difference.txt");
int printArrs()
{
    for (int i = 1; i < N+1; i++)
    {
        std::cout << "X: " << X[i] << "  " << "Y: " << Y[i] << std::endl;
    }
    return 0;
}
int fillVectors()
{
    for (int i = 1; i < N + 1; i++)
    {
        X[i] = X[i - 1] + h;
        Y[i] = X[i - 1] + h;
    }
    return 0;

}

int fillTempElDiff(int i, int j)
{
    tempArr[i][j] = std::abs(U[i][j] - UPred[i][j]);
    return 0;
}


int fillUArrayElement(int i, int j)
{
    if (i == 0 || i == N) U[i][j] = Y[j] * Y[j] - Y[j] + 1;
    else if (j == 0 || j == N) U[i][j] = X[i] * X[i] - X[i] + 1;
    else U[i][j] = 0.0;
    return 0;
}
int fillTempOnSolve(int i, int j)
{
    f = 4 + 2 * X[i] * X[i] - 2 * X[i] + 2 * Y[j] * Y[j] - 2 * Y[j];
    tempArr[i][j] = 0.25 * (U[i - 1][j] + U[i + 1][j] + U[i][j - 1] + U[i][j + 1] - h * h * f);
    return 0;
}
int fillUPredElement(int i, int j)
{
    UPred[i][j] = (X[i] * X[i] - X[i] + 1) * (Y[j] * Y[j] - Y[j] + 1);
    return 0;
}

int CopyFromTempToU(int i, int j)
{
    U[i][j] = tempArr[i][j];
    return 0;
}

int fillMatrix(int(*fillArrElement)(int, int), int start, int stop)
{
    for (int i = start; i < stop; i++)
    {
        for (int j = start; j < stop; j++)
        {
            fillArrElement(i, j);
        }
    }
    return 0;
}

int printMatrix(float arr[][N + 1])
{
    for (int i = 0; i < N + 1; i++)
    {
        for (int j = 0; j < N + 1; j++)
        {
            std::cout << int(arr[i][j] * 100) / 100.0 << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}

int solve()
{
#pragma omp parallel
    FidndingWasEnd = false;
    for (int k = 0; k < 500000; k++)
    {
        maxDiffInArr = 0.0;
        fillMatrix(*fillTempOnSolve, 1,N);
        fillMatrix(*CopyFromTempToU, 1, N);
        for (int i = 1; i < N; i++)
        {
            for (int j = 1; j < N; j++)
            {
                if (std::abs(U[i][j] - UPred[i][j]) > maxDiffInArr)
                {
                    maxDiffInArr = std::abs(U[i][j] - UPred[i][j]);
                }
            }
        }
        if (k % 100 == 0)
        {
            diffOut << k << " " << maxDiffInArr << std::endl;
        }
    }
    return 0;
}

int OutputTotxtFile()
{
    out << "X " << "Y " << "U"<<std::endl;
    for (int i = 0; i < N+1; i++)
    {
        for (int j = 0; j < N+1; j++)
        {
            out << i << " " << j << " " << U[i][j]<<std::endl;
        }
    }
    out.close();
    return 0;
}


int main()
{

    fillVectors();

    fillMatrix(*fillUArrayElement, 0, N + 1);
    fillMatrix(*fillUPredElement, 0, N + 1);

    printArrs();
    std::cout << std::endl;
    solve();
    std::cout << "Solution" << std::endl;
    printMatrix(U);
    std::cout << "Prediction solution (Upred)" << std::endl;
    printMatrix(UPred);
    std::cout << "Difference matrix" << std::endl;
    fillMatrix(*fillTempElDiff, 0, N + 1);
    printMatrix(tempArr);
    OutputTotxtFile();

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"


// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
