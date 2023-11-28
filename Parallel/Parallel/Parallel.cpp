// Parallel.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include "omp.h";

int const N = 32;//размеры матриц и массивов
//float const eps = 1.0; тут конечное число итераций
float const h = 1.0 / N;
float X[N + 1];
float Y[N + 1];

float solveMatrix[N + 1][N + 1];
float predictionMatrix[N + 1][N + 1];
float solveBufferMatrix[N + 1][N + 1];

float f;
bool FidndingWasEnd;
float maxDiffInArr;

std::fstream out("ANSWER.txt");
std::fstream diffOut("difference.txt");

int fillVectors()
{
    //заполняет векторы X и Y по формуле
    for (int i = 1; i < N + 1; i++)
    {
        X[i] = X[i - 1] + h;
        Y[i] = X[i - 1] + h;
    }
    return 0;

}

int fillTempElDiff(int i, int j)
{
    //получает i j -- координаты элемента
    //после чего из массива решения по i,j вычитается элемент их массива точного решения
    //это все записывается в матрицу-буффер
    solveBufferMatrix[i][j] = std::abs(solveMatrix[i][j] - predictionMatrix[i][j]);
    return 0;
}


int fillUArrayElement(int i, int j)
{
    if (i == 0 || i == N) solveMatrix[i][j] = Y[j] * Y[j] - Y[j] + 1;
    else if (j == 0 || j == N) solveMatrix[i][j] = X[i] * X[i] - X[i] + 1;
    else solveMatrix[i][j] = 0.0;
    return 0;
}
int fillTempOnSolve(int i, int j)
{
    //f -- считается, используя массивы X и Y
    //записывает во временную матрицу нга место i j результат вычеслений, который Получен с помощью массивов
    //X, Y и массива Текущего решения
    f = 4 + 2 * X[i] * X[i] - 2 * X[i] + 2 * Y[j] * Y[j] - 2 * Y[j];
    solveBufferMatrix[i][j] = 0.25 * (solveMatrix[i - 1][j] + solveMatrix[i + 1][j] + solveMatrix[i][j - 1] + solveMatrix[i][j + 1] - h * h * f);
    return 0;
}
int fillUPredElement(int i, int j)
{
    //заполеняет матрицу точного решения
    predictionMatrix[i][j] = (X[i] * X[i] - X[i] + 1) * (Y[j] * Y[j] - Y[j] + 1);
    return 0;
}

int CopyFromTempToU(int i, int j)
{
    //Копирование из массива-буффера в массив решения(для позиции i j)
    solveMatrix[i][j] = solveBufferMatrix[i][j];
    return 0;
}

int fillMatrix(int(*fillArrElement)(int, int), int start, int stop)
{
    //Применяет функцию, переданную сюда к каждому элементу массива(от начала до конца)
    //на массив ссылка идет прямо из функции
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
    //выводит элементы матрицы с двойной точностью
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
    //параллельность
#pragma omp parallel num_threads(8)
    //FidndingWasEnd = false;
    
    //у меня тут конечное чимсло итераций, чтобы не вподало в ступор
    for (int k = 0; k < 500000; k++)
    {
        //на каждой итерации найденная ошибка обнуляется
        //заполняется матрица-буффер, там все вычисляется на основе матрицы-решения
        //(на первой итерации при k=0 матрица решения состоит из нулей и границы)
        //матрица-буффер копируется в матрицу решения
        maxDiffInArr = 0.0;
        fillMatrix(*fillTempOnSolve, 1,N);
        fillMatrix(*CopyFromTempToU, 1, N);
        //вычисляет максимальную ошибку
        for (int i = 1; i < N; i++)
        {
            for (int j = 1; j < N; j++)
            {
                if (std::abs(solveMatrix[i][j] - predictionMatrix[i][j]) > maxDiffInArr)
                {
                    maxDiffInArr = std::abs(solveMatrix[i][j] - predictionMatrix[i][j]);
                }
            }
        }
        if (k % 1000 == 0)
        {
            diffOut << k << " " << maxDiffInArr << std::endl;
            std::cout << k << " " << maxDiffInArr << std::endl;
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
            out << i << " " << j << " " << solveMatrix[i][j]<<std::endl;
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

    //printArrs();
    std::cout << std::endl;
    solve();
    std::cout << "Solution" << std::endl;
    printMatrix(solveMatrix);
    std::cout << "Prediction solution (Upred)" << std::endl;
    printMatrix(predictionMatrix);
    std::cout << "Difference matrix" << std::endl;
    fillMatrix(*fillTempElDiff, 0, N + 1);
    printMatrix(solveBufferMatrix);
    OutputTotxtFile();

}
